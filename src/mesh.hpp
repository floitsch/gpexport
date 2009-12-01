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
 * data-structures for meshes.
 */

#ifndef ___HPP_GPEXPORT_MESH
#define ___HPP_GPEXPORT_MESH

#include <maya/MPoint.h>
#include <maya/MFloatVector.h>

#include <vector>
#include <string>

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
    };

} // namespace GPTranslation
#endif // include guard
