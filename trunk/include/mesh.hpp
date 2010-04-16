/*
 * 2004/06/24
 * Florian Loitsch
 *
 * data-structures for meshes.
 */

#ifndef ___HPP_GPEXPORT_MESH
#define ___HPP_GPEXPORT_MESH

#include <vector>
#include <string>

#include <maya/MPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>


#include "basic_types.hpp"
#include "material.hpp"

namespace GPTranslation
{
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

    struct Vertex
    {
        Vertex() : positionIndex(0), normalIndex(0), uvIndex(0) {}
        int32 positionIndex; // == weightIndex (if weights are exported)
        int32 normalIndex;
        int32 uvIndex;

		bool operator==(const Vertex& other)
		{
			return (
				positionIndex == other.positionIndex
			&&  normalIndex   == other.normalIndex
			&&  uvIndex       == other.uvIndex 
			);
		}
    };

    typedef std::vector<std::vector<Float2> > UVs;
    
    // note, that we loose the "face"-property of Maya.
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

		//- [11/26/2009 yys] for face/polygon materials 
		std::vector<std::string> shaderSets;
		std::vector<int> mtrIndexOfTriangle;//mtrIndexOfTriangle[i] = mtrIndex;
    };

} // namespace GPTranslation
#endif // include guard
