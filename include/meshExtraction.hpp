/*
 * 2004/06/27
 * Florian Loitsch
 *
 * extracts Meshes out of Maya
 */

#ifndef ___HPP_GPEXPORT_MESH_EXTRACTION
#define ___HPP_GPEXPORT_MESH_EXTRACTION

#include <maya/MDagPath.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MItMeshPolygon.h>

#include <vector>

#include "gpBase.hpp"
#include "materialExtraction.hpp"
#include "mesh.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    class MeshExtractor
	: protected virtual GPBase, // yes: it's already inherited by MaterialExtractor. just more explicit.
	  protected MaterialExtractor
    {
    public:
	/**
	 * extracts all chosen meshes. Choice is done in "shouldMeshBeExported".
	 */
	std::vector<Mesh> ExtractMeshes() const;

    protected:
	/**
	 * returns true, if the given mesh-path should be exported.
	 * this method is responsible for making the difference between "export all"
	 * and "export selection"...
	 *
	 * default just returns true.
	 */
	virtual bool shouldMeshBeExtracted(const MDagPath& dagPath) const
	    { return true; }

	/**
	 * returns true, if the positions should be extracted.
	 * 
	 * default-implementation returns true.
	 */
	virtual bool doExtractPositions() const
	    { return true; }

	/**
	 * returns true, if weights should be extracted.
	 * 
	 * default-implementation returns true.
	 */
	virtual bool doExtractWeights() const
	    { return true; }
	
	/**
	 * returns true, if the normals should be extracted.
	 * 
	 * default-implementation returns true.
	 */
	virtual bool doExtractNormals() const
	    { return true; }
		
	/**
	 * returns true, if UVs should be extracted.
	 * 
	 * default-implementation returns true.
	 */
	virtual bool doExtractUVs() const
	    { return true; }
		
	/**
	 * returns true, if Materials should be extracted.
	 * 
	 * default-implementation returns true.
	 */
	virtual bool doExtractMaterials() const
	    { return true; }
		
    private:
	std::vector<Int3> convertObjectToPoly(MIntArray objectIndices, MIntArray polyIndices) const;
	SkinningData extractSkinningData(const MDagPath mayaOutputMeshPath) const;

	std::vector<MPoint> extractPositions(const MFnMesh& mayaMesh) const;
	std::vector<MFloatVector> extractNormals(const MFnMesh& mayaMesh) const;
	UVs extractUVs(const MFnMesh& mayaMesh) const;
	std::vector<Int3> extractPolyTriangles(MItMeshPolygon& polyIt, uint32 vertexOffset) const;
	std::vector<Vertex> extractPolyVertices(const MFnMesh& mayaMesh, MItMeshPolygon& polyIt) const;
	Mesh extractMesh(const MDagPath& mayaMeshPath) const;
    
	void ExtractShaderSetAndShaderVertexIndices(const MDagPath& mayaMeshPath, 
		std::vector<std::string>& shaderSet_, std::vector<int> &shaderVertexIndices_) const;

	};
} // namespace GPTranslation
#endif // include guard
