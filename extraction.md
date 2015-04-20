# Extraction #

Maya is not very coder-friendly what extraction is concerned. This page presents the most basic steps to extract a scene.

As a sidenote: Be aware that (AFAIK) any new plugin could break any (more sophisticated) exporter. In other words: one can never really finish an exporter, as there might come a new plugin, that forces an update of the exporter.

## Mesh ##
Let's start with the struct that will contain the mesh's information:
```
struct Mesh
{
  std::vector<Material>       materials;
  std::string                 name;
  std::vector<MPoint>         positions;
  std::vector<MFloatVector>   normals;
  UVs                         uvs;
  SkinningData                skinningData;
  std::vector<Vertex>         vertices;
  std::vector<Int3>           indices;
};
```

The obvious fields are `name`, `vertices`, `indices` and `materials`. Each vertex itself has 3 fields, resp. pointing to an entry in the positions, normals and uvs. The position-index is also used to index into the influences-array, which itself is stored in the `skinningData`.

Most of these entries will be explained in more detail later on.

The easiest way of getting all meshes is by iterating over them. The `MItDag` class does just that. Here's an example (this time taken from `meshExtraction.cpp`):

```
for (MItDag dagIt(MItDag::kDepthFirst, MFn::kMesh, &status);
     !dagIt.isDone();
     dagIt.next())
{
    MDagPath currentMayaMeshPath;
    status = dagIt.getPath(currentMayaMeshPath);
    checkStatus("couldn't access path (dagIt.getPath)");

    if (!shouldMeshBeExtracted(currentMayaMeshPath))
    {
        cout << "  ignoring "
             << currentMayaMeshPath.partialPathName().asChar()
             << endl;
        continue;
    }

    Mesh currentMesh = extractMesh(currentMayaMeshPath);
    extractedMeshes.push_back(currentMesh);
}
return extractedMeshes;
```

`MItDag` iterates over all Maya-items. Depending on the parameters it does either a depth-first-, or breadth-first-search. The additional filter-parameter limits the search to the specified type (in this case `MFn::kMesh`).
We then just need to use the Mesh-function-set `MFnMesh` to access each of the found `MDagPaths`.
If the mesh is animated one should put the model into its correct frame or its [Bind-Pose](extraction#Bind-Pose.md).

### Positions ###

The vertex positions is the most basic information one can extract out of Maya. As it is needed by Maya itself, it is quite easy to access the position-information.

Supposing the mesh is in its correct frame the position-extraction is a very short:
```
MPointArray mayaPositions;
status = mayaMesh.getPoints(mayaPositions, MSpace::kWorld);
```

### Normals ###

Again. Extraction is quite easy. However, we don't know yet, how to join the positions and normals. (**No**: `position[i]` does not have `normal[i]` as normal). We'll need Vertex-information to do that.

```
MFloatVectorArray mayaNormals;
status = mayaMesh.getNormals(mayaNormals);
```


### UVs ###
Every mesh can have several UV-sets, which are accessed by their name. Furthermore each UV-set is seperated in two arrays: the Us and the Vs.

Compared to the positions and normals it's slightly longer, but still very easy: We get all set-names, and iterate over them, extracting all UVs.

Here is the typedef, that will hold the UVs:
```
typedef std::vector<std::vector<Float2> > UVs;
```

and here the code for the extraction:
```
MStringArray uvSetNames;
status = mayaMesh.getUVSetNames(uvSetNames);
checkStatus("couldn't extract UV-names");

for (uint32 i = 0; i < uvSetNames.length(); ++i)
{
    cout << "   uv-name: " << uvSetNames[i] << endl;
    
    MFloatArray us;
    MFloatArray vs;
    std::vector<Float2> currentUVSet;

    status = mayaMesh.getUVs(us, vs, &uvSetNames[i]);
    result.reserve(us.length());
    for (uint32 i = 0; i < us.length(); ++i)
    {
        Float2 currentUV;
        currentUV.u = us[i];
        currentUV.v = vs[i];
        currentUVSet.push_back(currentUV);
    }
    result.push_back(currentUVSet);
}
```

### Skinning Data ###
First the structs that will hold the data (from `mesh.hpp`):
```
// the boneIndex references a joint in SkinningData::jointNames
struct Influence
{
    uint32  boneIndex; // points to a name in "SkinningData.jointNames"
    float   weight;
};

typedef std::vector<Influence> Influences;


// for every vertex with positionIndex i the weights are in influences[i].
// if there aren't any weights for the mesh, the influences-vector is of
// size 0. (so don't index blindly into the vector).
//
// the influences index into the jointNames-vector, which might be
// different for every mesh.
struct SkinningData
{
    std::vector<std::string>    jointNames;
    std::vector<Influences>     influences;
};
```

The only not obvious field is the `jointNames`-vector. We need this vector, to achieve the mapping between animation-tracks and the influences: The name is used as identifier for each joint. Rather than putting the name into every influence, we just index into the `jointNames`-vector.

Once something touches animation, it starts getting difficult. And that's true for the skinning-data too. There's no easy way to get to the skin-cluster associated with the mesh. One could search the input-plugs and find the skin-cluster from there on. That's the way proposed by greggman on his [site](http://games.greggman.com/game/maya_mel_api_tips__hints__and_other_random_stuff/).

`gpExport` (as well as for instance MS's X-Exporter) takes an easier approach (which is less efficient): For every mesh we iterator over all dependency-nodes, filtering for skin-clusters. For every found skin-cluster, we look at all output-connections. If the current mesh is an output-shape of the skin-cluster, we found our skin-cluster:

```
 for (MItDependencyNodes depNodeIt(MFn::kSkinClusterFilter);
      !depNodeIt.isDone();
      depNodeIt.next())
 {
     MObject depNodeObject = depNodeIt.item();
     MFnSkinCluster mayaSkinCluster(depNodeObject, &status);
     checkStatus("couldn't acces skinCluster");

     // normally we should cycle through all outputConnections,
     // and look, if it's our mesh.
     // the following line is just shorter
     uint32 shapeIndex =
         mayaSkinCluster.indexForOutputShape(mayaOutputMeshPath.node(),
                                             &status);
     if (!status)
         continue; // our mesh is not an output for this skincluster
```

If a skin-cluster was found, we get all influence-object (should be joints), and store them in the `jointNames`-array.

```
MDagPathArray influenceObjectPaths;
mayaSkinCluster.influenceObjects(influenceObjectPaths, &status);
checkStatus("couldn't get influenceObjects");
for (uint32 i = 0; i < influenceObjectPaths.length(); ++i)
{
    skinningData.jointNames.push_back(
        influenceObjectPaths[i].partialPathName().asChar());
}
```

Then we get the influences:

```
 MObject mayaInputObject =
     mayaSkinCluster.inputShapeAtIndex(shapeIndex, &status);
 // numVertices() should be the same for input and output shape.
 skinningData.influences.resize(mayaOutputMesh.numVertices());

 // iterate over all points (= components in Maya) and get the
 // influences (= [boneIndex, weight])
 uint32 pointCounter = 0;
 for (MItGeometry geometryIt(mayaInputObject);
      !geometryIt.isDone();
      geometryIt.next(), ++pointCounter)
 {
     MObject mayaComponent = geometryIt.component();
     MFloatArray mayaWeightArray;
     uint32 numInfluences; // is going to be discarded

     mayaSkinCluster.getWeights(mayaOutputMeshPath,
                                mayaComponent,
                                mayaWeightArray,
                                numInfluences);
```

The important part is the call to getWeights of the skin-cluster. We iterate over all points (geometryIt) and get the skinweights with this function (one by one).

We then finally convert the retrieved weights into a convenient format:
```
for (uint32 j = 0; j < mayaWeightArray.length(); ++j)
{
    // I know: we are working with floats, but Maya has a
    // function to eliminate weights, that are too small
    if (mayaWeightArray[j] != 0)
    {
        Influence influence;
        influence.boneIndex = j;
        influence.weight = mayaWeightArray[j];
        skinningData.influences[pointCounter]
            .push_back(influence);
    }
}
```

### Triangles and Vertices ###
Triangles and vertices are not immediate to obtain, but the extraction is straightforward. One has to pass by Maya's faces to get them. One correct way to do that: using `MItMeshPolygon` as in the following example:

```
for (MItMeshPolygon polyIt(mayaMeshPath);
     !polyIt.isDone();
     polyIt.next())
{
    std::vector<Int3> polyTriangles =
        extractPolyTriangles(polyIt, vertexOffset);
```

#### Vertices ####

The positions, normals, etc. aren't yet useful, as we can't relate them. That's what the vertices are for. As already written, Maya stores its information in faces, and we need to pass by them. As we don't need the seperation into faces, we'll just merge all faces of a mesh.

The notion of "vertex" itself doesn't exist in Maya. Instead there are functions, to access all vertex-entries independantly (which makes sense, as not every vertex contains always the same entries). We are actually simplifying our life, by using always the same vertex-struct.

Here's finally the vertex-extraction method:

```
unsigned int nbVertices = polyIt.polygonVertexCount();
for (unsigned int i = 0; i < nbVertices; ++i)
{
    Vertex currentVertex;
    if (doExtractPositions() || doExtractWeights())
        currentVertex.positionIndex = polyIt.vertexIndex(i);

    if (doExtractNormals())
        currentVertex.normalIndex = polyIt.normalIndex(i);

    if (doExtractUVs())
    {
        MStringArray uvSetNames;
        mayaMesh.getUVSetNames(uvSetNames);
        // only the first set, for now.
        polyIt.getUVIndex(i, currentVertex.uvIndex, &uvSetNames[0]);
    }
```

#### Triangles ####

Maya only triangulates individual faces (=polygons), but not complete meshes. What is slightly more annoying: Maya's triangulation returns indices for the positions. We want the indices to index into the face-relative vertices. After all, if we can't get from the triangles to the vertices, the vertices are pretty useless.

The extraction-method isn't very complicated (have a look at the code for even more comments):

```
MPointArray nonTweakedPositions; // is going to be discarded here
MIntArray mayaIndices; // that's what we are looking for
polyIt.getTriangles(nonTweakedPositions, mayaIndices);
MIntArray polyIndices;
polyIt.getVertices(polyIndices);
result = convertObjectToPoly(mayaIndices, polyIndices);
// finally add the offset to each index
for (unsigned int i = 0; i < result.size(); ++i)
{
    Int3& currentTriangle = result[i];
    currentTriangle.i1 += vertexOffset;
    currentTriangle.i2 += vertexOffset;
    currentTriangle.i3 += vertexOffset;
}
```

Note, that we are merging all faces (we won't be able to tell from what face a triangle came frome).

And now the conversion-method:
```
uint32 i = 0;
while (i < objectIndices.length())
{
    Int3 currentTriangle;
    for (uint32 j = 0; j < 3; ++j)
    {
        int32 indexTarget = objectIndices[i];
        // search for the corresponding polyIndex
        uint32 k;
        for (k = 0; k < polyIndices.length(); ++k)
        {
            if (polyIndices[k] == indexTarget)
                break;
        }
        if (k == polyIndices.length()) error("couldn't find poly-Index");
        currentTriangle.i[j] = k;

        i++;
    }
    triangles.push_back(currentTriangle);
}
```

Basically we take every `positionIndex` (`objectIndices[i]`), and search the corresponding vertex, that has the same `positionIndex`. This should give a unique result (as positions are only used once per face).

### Material ###
Although materials are shared between different meshes, `gpExport` currently creates new materials for every mesh (in other words: the materials are duplicated). However, it should be easy to join them again (if needed).

Materials are stored in shaders and the one uses `getConnectedShaders` on a mesh to get them (`materialExtraction.cpp`):

```
mayaMesh.getConnectedShaders(instanceNumber,
                             shaderSets,
                             shaderVertexIndices);
```

Then we iterate over all shaders, and search for surfaceShader-plugs, as they are the nodes containing materials:
```
MFnDependencyNode depNode(shaderSet);
MPlug shaderPlug = depNode.findPlug("surfaceShader");

if (shaderPlug.isNull()) return materials; // empty vector

MPlugArray shaderPlugSources;
shaderPlug.connectedTo(shaderPlugSources, true, false);
```

For each connected shader, we get the shader-node, and convert the contained material-information into a more convenient format (I never really tested the conversion, but all other exporters I've seen do it this way.) Furthermore we only extract phong-, blinn- and lambert-shaders:

```
MPlug currentShaderPlugSource = shaderPlugSources[i];
MObject shaderNode = currentShaderPlugSource.node();
if (shaderNode.hasFn(MFn::kPhong))
    fillMaterial<MFnPhongShader>(&currentMaterial,
                                 shaderNode,
                                 "phong");
else if (shaderNode.hasFn(MFn::kBlinn))
    fillMaterial<MFnBlinnShader>(&currentMaterial,
                                 shaderNode,
                                 "blinn");
else if (shaderNode.hasFn(MFn::kLambert))
    fillMaterial<MFnLambertShader>(&currentMaterial,
                                   shaderNode,
                                   "lambert");
```

```
MaterialExtractor::fillMaterial(Material* pMaterial,
                                const MObject& shaderNode,
                                const std::string& shadingType) const
{
    T shader(shaderNode);
    std::string shadingMode;
    if (shadingType == "phong")
        pMaterial->shadingMode = "phong";
    else // blinn and lambert
        pMaterial->shadingMode = "gouraud";
        
    pMaterial->ambient = shader.ambientColor();
    pMaterial->diffuse = shader.diffuseCoeff() * shader.color();
    pMaterial->selfIllumination = shader.incandescence();

    pMaterial->shininess = getShininess(shader);
```

`getShininess` is parametrized by the type, and returns an appropriate value for each shader-type.

Finally `gpExport` extracts the textures:
```
MFnDependencyNode shaderDepNode(shaderPlugSource.node());

for (MItDependencyGraph shaderGraphIt(shaderPlugSource,
                                      MFn::kFileTexture,
                                      MItDependencyGraph::kUpstream);
     !shaderGraphIt.isDone();
     shaderGraphIt.next())
{
    MObject shaderTextureNode = shaderGraphIt.thisNode();
    MFnDependencyNode mayaTexture(shaderTextureNode);
    
    MString textureFile;
    mayaTexture.findPlug("fileTextureName").getValue(textureFile);
```

## Animation ##
Animation is way more complicated than static information. One difficulty lies in the fact, that people need different animation-data. Some need the positions at every frame, some want skeleton-transformations at each frame, some want them just at key-frames, etc.

`gpExport` only extracts skeleton-keyframes, but the used methods should be applicable to other extractions. Also: the `gpExport`'s extraction is not complete. I already found examples, where nothing got extracted... (feel free to submit patches)

At Wootsoft we used the same animation for different skeletons, and the extraction is therefore completely split up (which is not really unreasonable anyways).

### Bind-Pose ###
In order to export the mesh and skeleton correctly, the model needs to be put into its bind-pose first. Ideally the exporter should undo this modification.

This should have been easy, but was one of the most difficult tasks of the exporter: there's no easy way to get the bind-pose matrix.

We start by turning off the inverse kinematic (`bindPoseTool.cpp`):
```
MIkSystem::setGlobalSnap(false);
MIkSystem::setGlobalSolve(false);
```

We then run over all joints, find the bind-pose matrix, and put the joint into its initial transformation. If a joint is instanced, only the first instance is put back into bind-pose (which automatically puts back the others too). Also, we need to remember the current transformation, as we need to go back later.

```
MTransformationMatrix currentTransform = joint.transformation(&status);

joint2transform.first = jointPath.node();
joint2transform.second = currentTransform;
undoInfo.savedTransforms.push_back(joint2transform);

MMatrix bindPoseMatrix = getBindPoseMatrix(joint);
```

Now the difficult task: getting the bind-pose matrix. There are several methods of doing it, and `gpExport` unfortunately uses one that is not completly correct. For the correct way look at greggman's [site](http://games.greggman.com/game/maya_mel_api_tips__hints__and_other_random_stuff/) (again).

We first find the bindPose-plug and all connected nodes:
```
MPlug tempBindPosePlug = joint.findPlug("bindPose");

MPlugArray mapConnections;
tempBindPosePlug.connectedTo(mapConnections, false, true);
```

we then get the first found connection, search for the `xformMatrix`-attribute, and finally extract it:

```
MPlug bindPosePlug = mapConnections[0];

// this node should be a "dagPose"-node (in case you want to look it
// up in the help)
MFnDependencyNode bindPoseNode(bindPosePlug.node());

// and as such, has the "xformMatrix"-attribute.
MObject xformMatrixAttribute = bindPoseNode.attribute("xformMatrix");

MPlug localTransformPlug(bindPosePlug.node(), xformMatrixAttribute);
// xformMatrix is an array. to get our localmatrix we need to select
// the same index, as our bindPosePlug (logicalIndex()).
localTransformPlug.selectAncestorLogicalIndex(
                    bindPosePlug.logicalIndex(), xformMatrixAttribute);

MObject localMatrixObject;
localTransformPlug.getValue(localMatrixObject);
```

I don't know if this is really necessary, but it can't hurt: syncing the meshes:
```
for (MItDag dagIt(MItDag::kDepthFirst, MFn::kMesh, &status);
     !dagIt.isDone();
     dagIt.next())
{
    cout << ".";
    MDagPath meshPath;
    dagIt.getPath(meshPath);
    MFnMesh mesh(meshPath.node());
    mesh.syncObject();
}
```

Undoing the information is much easier:
```
for (BindPoseUndoInformation::JointMatrixVector::const_iterator
       it = undoInfo.savedTransforms.begin();
    it != undoInfo.savedTransforms.end();
    ++it)
{
    cout << ".";
    status = MFnIkJoint(it->first).set(it->second);
    if (!status)
        cout << "prob in undoInfo" << endl;
}

MIkSystem::setGlobalSnap(undoInfo.ikSnap);
MIkSystem::setGlobalSolve(undoInfo.ikSolve);

cout << endl;

syncMeshes();
```

As `gpExport` already does all this for you, I advise using its implementation.

### Skeleton ###
Skeletons are essentially hierarchical transformations: a transformation-matrix and its children. In addition we store the joint-name, as we'll use it as identifier. Eventhough not necessary we keep the world-transfrom too (`skeleton.hpp`):

```
struct JointNode
{
    std::string             name;
    std::vector<JointNode>  children;
    MMatrix                 localTransform;

    // is not necessary (as we have the hierarchy,
    // and the localTransform).
    // only included to avoid recalculations.
    MMatrix                 worldTransform;
};

struct Skeleton
{
    std::vector<JointNode> rootNodes;
};
```

All sequent code-snippets of this subsection are taken from `skeletonExtraction.cpp`.

The extraction itself is not that difficult: for each root-joint, we recursively extract the hierarchy.

```
for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
     !dagIt.isDone();
     dagIt.next())
{
    MDagPath jointPath;
    dagIt.getPath(jointPath);
    if (shouldJointBeExtracted(jointPath))
        skeleton.rootNodes.push_back(
            extractSkeletonHierarchy(jointPath, 2));
    // children have already been treated by extractSkeletonHierarchy,
    // or are intentionally ignored
    dagIt.prune();
```

The `extractSkeletonHierarchy`-method first gets the joint-name. Then the local transformation, and the world transform. Finally it iterates over its children:

```
currentJointNode.name = jointPath.partialPathName().asChar();

MFnTransform transform(jointPath);
currentJointNode.localTransform = transform.transformation().asMatrix();
currentJointNode.worldTransform = jointPath.inclusiveMatrix();
for (uint32 i = 0; i < jointPath.childCount(); ++i)
{
    if (jointPath.child(i).hasFn(MFn::kJoint))
    {
        MDagPath childPath;
        childPath.set(jointPath);
        childPath.push(jointPath.child(i));
        if (!shouldJointBeExtracted(childPath))
            continue; // prune tree
        JointNode childJointNode = extractSkeletonHierarchy(childPath,
                                                            depth + 1);
        currentJointNode.children.push_back(childJointNode);
```

### Tracks ###
`gpExport` gets the tracks (the keyframes and their transformations) for all joints. The struct holding this information has 3 additional fields: `fps`, `start` and `end`. `fps` obviously holds the frames per second. This also means, that some Maya-fps-settings can't be correctly extracted (for instance, all `fps` smaller the one). `start` and `end` are redundant fields, as the beginning and end of the animation can be found in the tracks, but often simplifies the output. (`animation.hpp`)

```
struct KeyFrame
{
    MTime   time;
    MMatrix transformation;
};

typedef std::vector<KeyFrame> Track;
typedef std::map<std::string, Track> StringTrackMap;

struct Animation
{
    uint32              fps;
    MTime               start; // first keyframe. redundant information
                               // (could be extracted out of the tracks)
    
    MTime               end;   // last keyframe. redundant information
                               // (could be extracted out of the tracks)
    
    StringTrackMap      tracks;
};
```

The extraction is done in three steps:
  * get all animated joints.
  * get the key-times for all joints.
  * and finally get the transformations.

The animated joints are quite easy to obtain:

```
for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
     !dagIt.isDone();
     dagIt.next())
{
    MDagPath jointPath;
    dagIt.getPath(jointPath);

    if (!MAnimUtil::isAnimated(jointPath))
    {
        cout << "not animated" << endl;
        continue;
    }

    jointPaths.push_back(jointPath);
```

The key-times aren't that difficult neither: for every animated joint we look for the animated plugs, get the `MFnAnimCurves`, which hold all the times, and extract them:

```
MPlugArray animatedPlugs;
MAnimUtil::findAnimatedPlugs(jointPath, animatedPlugs);
for (unsigned int i = 0; i < animatedPlugs.length(); ++i)
{
    MObjectArray curves;
    MAnimUtil::findAnimation(animatedPlugs[i], curves);
    for (unsigned int j = 0; j < curves.length(); ++j)
    {
        MFnAnimCurve curve(curves[j]);
        for (unsigned int k = 0; k < curve.numKeys(); ++k)
            jointTimes.insert(curve.time(k));
    } // for curves
} // for animatedPlugs
```

Then we create a times->joint map, so all times are sorted (and we know what joint is animated at each time). We need a multi-map here, as several joints are usually animated at the same time (`animationExtraction.hpp/cpp`):

```
typedef std::multimap<MTime, uint32> Times2JointMap;
```

```
for (uint32 i = 0; i < animatedJointPaths.size(); ++i)
{
    MDagPath jointPath = animatedJointPaths[i];
    
    std::set<MTime> jointTimes = extractKeyFrameTimes(jointPath);
    
    // the jointTimes-set contains all key-times this joint is 
    // animated with.
    for (std::set<MTime>::const_iterator it = jointTimes.begin();
         it != jointTimes.end();
         ++it)
        // insert pairs [time, {animatedJointPath}Index] into
        // the times2JointMap.
        times2JointMap.insert(Times2JointMap::value_type(*it, i)); 
}
```

Once all key-times have been extracted, we set Maya's animation-time to every time of this map, and get the transformations for every joint that is animated.

```
// save initial time to go back after we have extracted the
// transformations.
const MTime initialTime = MAnimControl::currentTime();

// mayaTime = time Maya is set to. (property stays true in the
// following loop). I avoided naming it "currentTime", as i name
// my loop-vars currentXX.
MTime mayaTime = initialTime;

// times2Joint is a multiset and hence sorted.
for (Times2JointMap::const_iterator it = times2JointMap.begin();
     it != times2JointMap.end();
     ++it)
{
    if (mayaTime != it->first)
    {
        nbKeyTimes++; // just for logging
        MAnimControl::setCurrentTime(it->first);
        mayaTime = it->first;
    }
    Track&   currentTrack     = result.tracks[jointNames[it->second]];
    MDagPath currentJointPath = animatedJointPaths[it->second];
    KeyFrame newKeyFrame;
    newKeyFrame.time = it->first;
    // extract the transformation for this joint.
    MFnTransform transform(currentJointPath);
    newKeyFrame.transformation = transform.transformation().asMatrix();
    currentTrack.push_back(newKeyFrame);
}
// back to initial time
MAnimControl::setCurrentTime(initialTime);
```