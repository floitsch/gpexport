/*
Copyright (c) 2004 Florian Loitsch

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Extracts the whole mesh.
 */

#include "meshExtraction.hpp"

#include <maya/MFnSkinCluster.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MStringArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MPointArray.h>
#include <maya/MItDag.h>

namespace GPTranslation
{
    // objectIndices index into fnMesh.getPoints(). we want the result to index
    // into the polyIndices.
    // so for every objectIndex, we search the corresponding polyIndex, that
    // points to the same point. (polyIndex is just another redirection).
    // the reason for this method is, that all the rest of our export is done
    // using face-indices, but the triangulation points to objectIndices
    // (there's no Maya-function, that does the job).
    // btw: as we talk about triangles, we put them into groups of 3 (Int3).
    std::vector<Int3>
    MeshExtractor::convertObjectToPoly(MIntArray objectIndices, MIntArray polyIndices) const
    {
        std::vector<Int3> triangles; // point into polyIndices.
        triangles.reserve(polyIndices.length() / 3);

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
        return triangles;
    }


    // this is not the best way of doing, but is quite easy to understand, and
    // as far as I remember MS's X-exporter does it the same way: Loop over all
    // SkinClusters, and look, if they influence this mesh.
    //
    // for the correct way:
    //    http://www.greggman.com/pages/mayastuff.htm#skin
    SkinningData
    MeshExtractor::extractSkinningData(const MDagPath mayaOutputMeshPath) const
    {
        MFnMesh mayaOutputMesh(mayaOutputMeshPath, &status);

        // first find skinclusters.
        // if any of those skinclusters influence our mesh, extract the weights
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

            // note, that we only get here once (there's a "return"
            // at the end of the scope).

            cout << "   skincluster found" << std::endl;

            SkinningData skinningData;

            // - get the names of the influence-objects (joints)
            MDagPathArray influenceObjectPaths;
            mayaSkinCluster.influenceObjects(influenceObjectPaths, &status);
	    checkStatus("couldn't get influenceObjects");
            for (uint32 i = 0; i < influenceObjectPaths.length(); ++i)
            {
                skinningData.jointNames.push_back(
		    influenceObjectPaths[i].partialPathName().asChar());
            }

            // - get the influences
	    
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

                // put them into a more convenient format
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
            }
            return skinningData;
        }
        // no skinCluster for this mesh
	// a virgin SkinningData represents perfectly an un-influenced mesh.
        return SkinningData();
    } // extractSkinningData

    // this is simple. Maya provides a good interface here.
    std::vector<MPoint>
    MeshExtractor::extractPositions(const MFnMesh& mayaMesh) const
    {
	std::vector<MPoint> result;
	
	MPointArray mayaPositions;
	status = mayaMesh.getPoints(mayaPositions, MSpace::kWorld);
	checkStatus("couldn't extract positions");
	cout << "   #positions: " << mayaPositions.length() << endl;
	// transform them into our format
	result.reserve(mayaPositions.length());
	for (unsigned int i = 0; i < mayaPositions.length(); ++i)
	{
	    result.push_back(mayaPositions[i]);
	}
	return result;
    }

    // again. Maya provides nice ways of doing this.
    std::vector<MFloatVector>
    MeshExtractor::extractNormals(const MFnMesh& mayaMesh) const
    {
	std::vector<MFloatVector> result;
	    
	MFloatVectorArray mayaNormals;
	status = mayaMesh.getNormals(mayaNormals);
	checkStatus("couldn't extract normals");
	cout << "   #normals: " << mayaNormals.length() << endl;

	result.reserve(mayaNormals.length());
	for (unsigned int i = 0; i < mayaNormals.length(); ++i)
	{
	    result.push_back(mayaNormals[i]);
	}

	return result;
    }

    // still easy. thanks Maya.
    UVs
    MeshExtractor::extractUVs(const MFnMesh& mayaMesh) const
    {
	UVs result;

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
	
	return result;
    }

    // returns the triangles for the given polygon.
    // as all polygons are merged, the indices need to be adjusted by
    // the vertexOffset.
    // polyIt must be non-const, although it shouldn't be modified in
    // this method.
    std::vector<Int3>
    MeshExtractor::extractPolyTriangles(MItMeshPolygon& polyIt,
					uint32 vertexOffset) const
    {
	std::vector<Int3> result;
	
	// - Triangles
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
	return result;
    }

    // note, that there are several ways to proceed from now on:
    // to get the positions:
    //   polyIt.getVertices(MIntArray)
    //   mesh.getPolygonVertices(iPoly, MIntArray)
    //   polyIt.getPoints(MPointArray)
    //   foreach vertexIndex: polyIt.vertexIndex(vertexIndex)
    //
    // the same for normals, UVs,...
    //
    //   advantage of using the indices (getVertices(), getPolygonVertices(),
    //   vertexIndex()):
    //   if 2 vertices are the same, their indices seem to be equal (and the
    //   comparison is over integers). especially the normals are nice: when
    //   "average"ing normals, they seem to point to the same normal.
    //
    // Anyways: use the help for more info, and don't forget, that
    // "getTriangles()" only works for object-Indices, and that the returned
    // positions are not tweaked.
    //
    // polyIt should not be modified, but unfortunately must be passed as
    // non-const.
    std::vector<Vertex>
    MeshExtractor::extractPolyVertices(const MFnMesh& mayaMesh,
				       MItMeshPolygon& polyIt) const
    {
	std::vector<Vertex> result;
	
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
	    // nothing to do for weigthIndex, as the positionIndex
	    // equals the weightIndex.

	    result.push_back(currentVertex);
	}
	
	return result;
    }
    
    // extractMesh.
    // straight-forward:
    //  first get model-relativ data (skip a point, if options say otherwise)
    //   - name
    //   - positions (= points)
    //   - normals
    //   - UVs
    //   - SkinningData (weights, ...)
    //   - Materials
    //  then, for each face:
    //   - construct Vertices, that point to the model-relative data.
    //   - extract the triangles
    //
    // Note, that the extracted mesh needs to be optimized: a boundary-vertex
    // (of 2 faces), will exist twice in our representation.
    Mesh
    MeshExtractor::extractMesh(const MDagPath& mayaMeshPath) const
    {
        Mesh currentMesh;

        MFnMesh mayaMesh(mayaMeshPath, &status);
	checkStatus("couldn't access mesh");

        // - name
	currentMesh.name = mayaMeshPath.partialPathName().asChar();
	cout << "  name: " << currentMesh.name << endl;

        // - positions
	currentMesh.positions = extractPositions(mayaMesh);

        // - normals
        if (doExtractNormals())
	    currentMesh.normals = extractNormals(mayaMesh);

        // - UVs (we are currently only exporting the first UV-set)
        if (doExtractUVs())
	    currentMesh.uvs = extractUVs(mayaMesh);

        // - SkinningData
        if (doExtractWeights())
            currentMesh.skinningData = extractSkinningData(mayaMeshPath);

	// - Materials
	if (doExtractMaterials())
	    currentMesh.materials = ExtractMaterials(mayaMeshPath);

        // ===========================
        // now the face-relative stuff (remember: face == polygon in Maya)

        cout << "   #polygons: " << mayaMesh.numPolygons() << endl;
        unsigned int nbTriangles = 0;  // only needed for logging
        unsigned int vertexOffset = 0; // needed, because we collapse the polygons
        for (MItMeshPolygon polyIt(mayaMeshPath);
             !polyIt.isDone();
             polyIt.next())
        {
	    std::vector<Int3> polyTriangles =
		extractPolyTriangles(polyIt, vertexOffset);

	    currentMesh.indices.insert(currentMesh.indices.end(),
				       polyTriangles.begin(),
				       polyTriangles.end());
	    nbTriangles += polyTriangles.size();

	    std::vector<Vertex> polyVertices =
		extractPolyVertices(mayaMesh, polyIt);
	    currentMesh.vertices.insert(currentMesh.vertices.end(),
					polyVertices.begin(),
					polyVertices.end());
            // increase the vertexOffset for the next polygon.
	    vertexOffset += polyVertices.size();
        } // polyIt

	cout << "   #Vertices: " << currentMesh.vertices.size() << std::endl;
	cout << "   #Triangles: " << currentMesh.indices.size() << std::endl;

        return currentMesh;
    } // extractMesh()


    // runs over all meshes, und extract them (if wanted).
    std::vector<Mesh>
    MeshExtractor::ExtractMeshes() const
    {
        cout << " extracting meshes" << std::endl;
        std::vector<Mesh> extractedMeshes;

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
    }
} // namespace GPTranslation
