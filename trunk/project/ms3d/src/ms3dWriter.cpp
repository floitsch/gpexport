/*
 * Exports to MS3D-format.
 *
 * I never needed the ms3d-format, and this file is only
 * here as an example. I tested the exported file using
 * the IrrLicht-engine: http://irrlicht.sourceforge.net
 * and it worked fine.
 * My examples were pretty small though, and I'm not
 * guaranteeing anything.
 *
 * The MS3D-format is documented in their SDK:
 * http://www.swissquake.ch/chumbalum-soft/ms3d/download.html
 *
 * the ms3d.h-file is nearly a copy-paste of one of their files. (I
 * hope I don't break any copyright...)
 */
#include "ms3dWriter.hpp"

#include <stdio.h>
#include <string.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>

#include "ms3d.h"

namespace MS3DTranslation
{
    void
    MS3DWriter::writeHeader()
    {
	std::cout << " Writing Header" << std::endl;
	ms3d_header_t header;
	strncpy(header.id, "MS3D000000", 10);
	header.version = 4;
	fwrite(header);
    }

    int
    MS3DWriter::nbVertices(const Scene& scene)
    {
	int sum = 0;
	for (unsigned int i = 0; i < scene.meshes.size(); ++i)
	    sum += scene.meshes[i].positions.size();
	return sum;
    }

    // MS3D refers to joints by index, and not by name.
    // using this method, we calculate the index, the joints will
    // have, when we extract them: nameVector[i] contains the joint with index i.
    void
    MS3DWriter::fillJointNameVector(const JointNode& node,
				    std::vector<std::string>* nameVector)
    {
	nameVector->push_back(node.name);
	for (uint32 i = 0; i < node.children.size(); ++i)
	    fillJointNameVector(node.children[i], nameVector);
    }

    // maps a joint to the index it will have, when we extract it.
    const std::map<std::string, uint32>
    MS3DWriter::getNameIndexMap(const Scene& scene)
    {
	
	std::vector<std::string> nameVector;
	
	const Skeleton& skel = scene.skeleton;
	for (uint32 i = 0; i < skel.rootNodes.size(); ++i)
	    fillJointNameVector(skel.rootNodes[i], &nameVector);

	// transform it into a map (easier to access)
	std::map<std::string, uint32> result;
	for (uint32 i = 0; i < nameVector.size(); ++i)
	    result[nameVector[i]] = i;

	return result;
    }
    
    // let's face it: MS3D is just not sophisticated enough.
    // A vertex can only be influenced by at most one bone. (this is certainly not true
    // for Maya, or other engines/file-formats).
    // Going to find the bone, that influences the most the given vertex.
    char
    MS3DWriter::getBoneIndex(uint32 vertexIndex,
			     const Mesh& mesh,
			     const std::map<std::string, uint32> nameIndexMap) // maps the jointName to the index it will have later on.
    {
	const uint32 weightIndex = mesh.vertices[vertexIndex].positionIndex;
	const SkinningData& skinning = mesh.skinningData;
	
	if (skinning.influences.size() == 0) return -1; // doesn't have weights

	const Influences& influences= skinning.influences[weightIndex];
	
	if (influences.size() == 0) return -1;

	float  maxWeight = 0;
	char jointIndex = -1;

	for (uint32 i = 0; i < influences.size(); ++i)
	{
	    if (influences[i].weight > maxWeight)
	    {
		maxWeight = influences[i].weight;
		const std::string& jointName = skinning.jointNames[influences[i].boneIndex];
		// so we got the name. translate the correct index.
		std::map<std::string, uint32>::const_iterator it = nameIndexMap.find(jointName);
		if (it != nameIndexMap.end())
		    jointIndex = (char) it->second;
		else
		    jointIndex = -1; // should not happen.
	    }
	}
	return jointIndex;
    }

    // just dumps the vertices of the given mesh. (MS3D needs reference counts,
    // but this isn't really an issue).
    void
    MS3DWriter::writeVertices(const Mesh& meshToWrite,
			      const std::map<std::string, uint32> nameIndexMap)
    {
	unsigned int nbVertices = meshToWrite.vertices.size();

	//int referenceCounts[nbVertices];
	std::vector<int> referenceCounts; referenceCounts.resize(nbVertices);

	for (unsigned int i = 0; i < nbVertices; ++i)
	    referenceCounts[i] = 0;
	
	for (unsigned int i = 0; i < meshToWrite.indices.size(); ++i)
	    for (unsigned int j = 0; j < 3; ++j)
		referenceCounts[meshToWrite.indices[i].i[j]]++;
	
	for (unsigned int i = 0; i < meshToWrite.vertices.size(); ++i)
	{
	    const Vertex& vertex = meshToWrite.vertices[i];
	    const MPoint& position = meshToWrite.positions[vertex.positionIndex];
	    
	    ms3d_vertex_t ms3dVertex;
	    ms3dVertex.flags = 0;
	    for (unsigned int j = 0; j < 3; ++j)
		ms3dVertex.vertex[j] = (float)position[j];
	    ms3dVertex.boneId = getBoneIndex(i, meshToWrite, nameIndexMap);
	    ms3dVertex.referenceCount = referenceCounts[i];

	    fwrite(ms3dVertex);
	}

    }

    // dumps all the vertices.
    // as the vertices are referring to bone-indices, we need
    // to create the jointName2index-map first.
    void
    MS3DWriter::writeVertices(const Scene& sceneToWrite)
    {
	// write number vertices first (sum of all vertices).
	unsigned int nbTotalVertices = 0;
	for (unsigned int i = 0; i < sceneToWrite.meshes.size(); ++i)
	    nbTotalVertices += sceneToWrite.meshes[i].vertices.size();

	fwrite((word)nbTotalVertices);

	// in order to do get the correct joint-index we need a mapping
	// from joint-name to index.
	std::map<std::string, uint32> nameIndexMap = getNameIndexMap(sceneToWrite);

	// now write the vertices
	for (unsigned int i = 0; i < sceneToWrite.meshes.size(); ++i)
	    writeVertices(sceneToWrite.meshes[i], nameIndexMap);
    }

    // just dumps all the vertices of the mesh.
    // note, that we associated the normals to the vertices during extraction.
    // MS3D associates them to the triangles. Therefore the extracted mesh is
    // clearly not optimal, as two vertices with the same position, will be
    // exported twice (which is not necessary in MS3D).
    unsigned int
    MS3DWriter::writeTriangles(const Mesh& meshToWrite,
			       unsigned int indexOffset,
			       byte groupIndex)
    {
	for (unsigned int i = 0; i < meshToWrite.indices.size(); ++i)
	{
	    const Int3& indices = meshToWrite.indices[i];
	    ms3d_triangle_t ms3dTriangle;
	    ms3dTriangle.flags = 0;
	    for (unsigned int j = 0; j < 3; ++j)
	    {
		const int vertexIndex = indices.i[j];
		const Vertex& vertex = meshToWrite.vertices[vertexIndex];
		ms3dTriangle.vertexIndices[j] = (word)(vertexIndex + indexOffset);

		int32 normalIndex = vertex.normalIndex;
		for (unsigned int k = 0; k < 3; ++k)
		    ms3dTriangle.vertexNormals[j][k] = (float)meshToWrite.normals[normalIndex][k];

		int32 uvIndex = vertex.uvIndex;

		// only get the first UV-set.
		ms3dTriangle.s[j] = meshToWrite.uvs[0][uvIndex].s;
		ms3dTriangle.t[j] = meshToWrite.uvs[0][uvIndex].t;
	    }
	    ms3dTriangle.smoothingGroup = 0;
	    ms3dTriangle.groupIndex = groupIndex;

	    fwrite(ms3dTriangle);
	}
	return meshToWrite.vertices.size();
    }

    // dumps all triangles.
    void
    MS3DWriter::writeTriangles(const Scene& sceneToWrite)
    {
	/*
	 * first get the total number of indices.
	 */
	unsigned int nbTotalIndices = 0;
	for (unsigned int i = 0; i < sceneToWrite.meshes.size(); ++i)
	    nbTotalIndices += sceneToWrite.meshes[i].indices.size();

	fwrite<word>((word)nbTotalIndices);

	/*
	 * now write the indices
	 */
	// the we "merge" the meshes, so we need to adjust the indices.
	unsigned int indexOffset = 0;
	// every mesh is a group
	byte groupIndex = 0;
	for (unsigned int i = 0; i < sceneToWrite.meshes.size(); ++i)
	{
	    indexOffset += writeTriangles(sceneToWrite.meshes[i], indexOffset, groupIndex++);
	}
    }

    // every Maya-mesh becomes a MS3D-group.
    void
    MS3DWriter::writeGroups(const Scene& sceneToWrite)
    {
	unsigned int nbMeshes = sceneToWrite.meshes.size();
	fwrite((word)nbMeshes);
	
	unsigned int triangleCounter = 0;
	for (unsigned int i = 0; i < nbMeshes; ++i)
	{
	    const Mesh& mesh = sceneToWrite.meshes[i];
	    fwrite<byte>(0); // flags
	    char name[32];
	    strncpy(name, mesh.name.c_str(), 32);
	    name[31] = '\0'; // strncpy doesn't necessarily terminate the string.
	    fwrite<char>(name, 32);

	    unsigned int nbIndices = mesh.indices.size();
	    fwrite<word>((word)nbIndices);
	    
	    for (unsigned int j = 0; j < nbIndices; ++j)
		fwrite<word>((word)triangleCounter++);
	    fwrite<char>(-1);
	}
    }

    // TODO.
    void
    MS3DWriter::writeMaterials(const Scene& sceneToWrite)
    {
	std::cout << " Writing Materials" << std::endl;
	fwrite((word)0); // currently nothing.
    }

    // dumps the rotation-part of the matrix, decomposed in x, y, z rotations.
    void
    MS3DWriter::writeRotation(const MMatrix& matrix)
    {
	const MTransformationMatrix transformationMatrix(matrix);
	const MEulerRotation rotation = transformationMatrix.eulerRotation();
	fwrite<float>((float)rotation.x);
	fwrite<float>((float)rotation.y);
	fwrite<float>((float)rotation.z);
    }

    // dumps the translation-part of the matrix.
    void
    MS3DWriter::writePosition(const MMatrix& matrix)
    {
	const MTransformationMatrix transformationMatrix(matrix);
	const MVector translation = transformationMatrix.translation(MSpace::kTransform);
	fwrite<float>((float)translation.x);
	fwrite<float>((float)translation.y);
	fwrite<float>((float)translation.z);
    }

    // dumps the rotational part of the keyframe.
    // note, that maya crashed quite often in this function. I have no idea why! A reproducable
    // way of crashing Maya was: load the plugin. export. unload. reload. export and crash when
    // accessing the keyFrame-time.
    // If you know why, drop me a mail...
    void
    MS3DWriter::writeKeyFrameRotation(const KeyFrame& keyFrame,
				      const MMatrix& inverse)
    {
	fwrite<float>((float)keyFrame.time.as(MTime::kSeconds));
	writeRotation(inverse * keyFrame.transformation);
    }

    // dumps the translational part of the keyframe.
    void
    MS3DWriter::writeKeyFramePosition(const KeyFrame& keyFrame,
				      const MMatrix& inverse)
    {
	fwrite<float>((float)keyFrame.time.as(MTime::kSeconds));
    	writePosition(inverse * keyFrame.transformation);
    }

    // dumps all the keyframes.
    // as we just extracted the transformation-matrix of each joint, we don't know, if it was a rotation
    // or translation. Hence we write both. This isn't really optimal, but shouldn't harm.
    void
    MS3DWriter::writeTrack(const Track& track,
			   const JointNode& node,
			   const MTime& start,
			   const MTime& end)
    {
	uint32 nbKeyFrames = track.size();

	if (nbKeyFrames == 0)
	{
	    fwrite<word>(0); // nbRotationFrames
	    fwrite<word>(0); // nbTranslationFrames
	    return;
	}

	// Maya uses the first keyframe for everything before the first time.
	// MS3D uses the original transformation (which is not necessarely the same).
	// so just copy the first keyframe to the start of the animation, so the
	// behaviour is same.
	KeyFrame firstFrame = track[0];
	firstFrame.time = start;
	// same is true for last frame.
	KeyFrame lastFrame = track[track.size() - 1];
	lastFrame.time = end;

	bool newStart = (track[0].time != start);
	bool newEnd = (track[nbKeyFrames - 1].time != end);
	
	char additionalFrames = 0;
	if (newStart) additionalFrames++;
	if (newEnd) additionalFrames++;

	fwrite<word>((word)(nbKeyFrames + additionalFrames)); // nbRotationFrames
	fwrite<word>((word)(nbKeyFrames + additionalFrames)); // nbTranslationFrames

	MMatrix inverseTransformation = node.localTransform.inverse();
	
	// rotation
	if (newStart)
	    writeKeyFrameRotation(firstFrame, inverseTransformation);
	for (uint32 i = 0; i < track.size(); ++i)
	    writeKeyFrameRotation(track[i], inverseTransformation);
	if (newEnd)
	    writeKeyFrameRotation(lastFrame, inverseTransformation);

	// position
	if (newStart)
	    writeKeyFramePosition(firstFrame, inverseTransformation);
	for (uint32 i = 0; i < track.size(); ++i)
	    writeKeyFramePosition(track[i], inverseTransformation);
	if (newEnd)
	    writeKeyFramePosition(lastFrame, inverseTransformation);
    }

    // recursively counts the nb of joints.
    uint32
    MS3DWriter::countJoints(const JointNode& node)
    {
	uint32 childrenNodesCount = 0;
	for (uint32 i = 0; i < node.children.size(); ++i)
	    childrenNodesCount += countJoints(node.children[i]);
	return childrenNodesCount + 1; // don't forget the current node 
    }

    // dumps the joint and its children.
    // note, that the animations are written here too.
    void MS3DWriter::writeJoints(const JointNode& node,
				 const std::string& parentName,
				 const Scene& scene)
    {
	fwrite<byte>(0); // flags
	
	char buffer[32];
	// IMPORTANT: if the name's longer than 32 chars,
	//    this could pose big troubles. (as unicity is
	//    not guaranteed anymore.
	strncpy(buffer, node.name.c_str(), 32);
	buffer[31] = '\0';
	fwrite<char>(buffer, 32);

	strncpy(buffer, parentName.c_str(), 32);
	buffer[31] = '\0';
	fwrite<char>(buffer, 32);

	writeRotation(node.localTransform);
	writePosition(node.localTransform);
	// wanted to write scene.animation.tracks[node.name] in the next
	// line. but due to const-issues, this isn't possible.
	const Animation& anim = scene.animation;
	StringTrackMap::const_iterator it = anim.tracks.find(node.name);
	writeTrack(it->second, node, anim.start, anim.end);

	for (uint32 i = 0; i < node.children.size(); ++i)
	    writeJoints(node.children[i], node.name, scene);
    }

    // writes all joints.
    void
    MS3DWriter::writeJoints(const Scene& sceneToWrite)
    {
	const Skeleton& skel = sceneToWrite.skeleton;
	uint32 nbJoints = 0;
	for (uint32 i = 0; i < skel.rootNodes.size(); ++i)
	    nbJoints += countJoints(skel.rootNodes[i]); // don't count the root-nodes

	fwrite<word>((word)nbJoints);

	for (uint32 i = 0; i < skel.rootNodes.size(); ++i)
	    writeJoints(skel.rootNodes[i], "", sceneToWrite);
    }

    // nothing special here.
    void
    MS3DWriter::writeAnimation(const Scene& sceneToWrite)
    {
	std::cout << " Writing Animation" << std::endl;
	const Animation& anim = sceneToWrite.animation;
	uint32 fps = anim.fps;
	fwrite<float>((float)fps);
	fwrite<float>((float)anim.start.as(MTime::kSeconds)); // currentTime (we take starttime)
	MTime length = anim.end - anim.start;
	fwrite<int>((int)(length.as(MTime::kSeconds)*fps)); // totalframes
	writeJoints(sceneToWrite);
    }

    // write the meshes, split into vertices, triangles, and groups.
    void
    MS3DWriter::writeMeshes(const Scene& sceneToWrite)
    {
	std::cout << " Writing Meshes" << std::endl;
	writeVertices(sceneToWrite);
	writeTriangles(sceneToWrite);
	writeGroups(sceneToWrite);
    }

    void
    MS3DWriter::WriteScene(const Scene& sceneToWrite )
    {
	std::cout << "Writing ms3d to " << m_config->getOutFile() << std::endl;
	FileWriter::WriteScene(sceneToWrite, m_config->getOutFile());
    }

    void
    MS3DWriter::doWriteScene(const Scene& sceneToWrite)
    {
	writeHeader();
	writeMeshes(sceneToWrite);
	writeMaterials(sceneToWrite);
	writeAnimation(sceneToWrite);
    }
} // namespace GPTranslation
