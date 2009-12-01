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
 * Optimizes the scene.
 */

#include "optimization.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    // needed, so we can put it into a map.
    struct vertexLess
    {
        bool operator()(const Vertex& v1, const Vertex& v2) const
        {
            if (v1.positionIndex != v2.positionIndex) return (v1.positionIndex < v2.positionIndex);
            if (v1.normalIndex   != v2.normalIndex)   return (v1.normalIndex   < v2.normalIndex);
            if (v1.uvIndex       != v2.uvIndex)       return (v1.uvIndex       < v2.uvIndex);
            return false;
        }
    };

    // small optimization of the mesh.
    // the idea is simple: remove duplicate vertices.
    // as every vertex has been extracted without looking outside it's face,
    //  boundary-vertices are often the same (exceptions are: different normals, uvs, ...).
    // this method removes these duplicates (and of course adjusts the indices).
    //
    // Could be, that just comparing the indices is not enough. But it worked for me most
    // of the time.. ;)
    void
    Optimizer::optimizeMesh(Mesh* meshToOptimize) const
    {
        std::cout << " optimizing mesh " << meshToOptimize->name << std::endl;
        std::vector<Vertex>& vertices = meshToOptimize->vertices;
        std::vector<Int3>&   indices  = meshToOptimize->indices;

        uint32 oldVertexCount = vertices.size();
        int* vertexRedirection = new int[vertices.size()]; // mapping from the old position to the new position in the vertices-vector.
        std::map<Vertex, int, vertexLess> vertexMapping;   // for each vertex points to its position in the new vertices-vector.
        int vertexCounter = 0;                             // counts distinct vertices (and hence points to the first free slot in the new vertices-vector).
        for (uint32 i = 0; i < vertices.size(); ++i)
        {
            if (vertexMapping.find(vertices[i]) == vertexMapping.end())
            {
                // this is a new vertex:
                //  - copy the vertex into its slot (vertices[vertexCounter]), which is the first free slot.
                //  - add it to our mapping, pointing to its position (= vertexCounter).
                //  - update the vertexRedirection-table
                //  - increase the free-slot-pointer (=vertexCounter).
                vertices[vertexCounter] = vertices[i];
                vertexMapping[vertices[i]] = vertexCounter;
                vertexRedirection[i] = vertexCounter;
                vertexCounter++;
            }
            else
            {
                // this vertex already exists. so just update the redirection.
                vertexRedirection[i] = vertexMapping[vertices[i]];
            }
        }
        vertices.resize(vertexCounter); // destroys the vertices at vertices[i], where i >= vertexCounter
        cout << "  reduced from " << oldVertexCount << " to " << vertexCounter << endl;

        // repair the indices
        for (uint32 i = 0; i < indices.size(); ++i)
        for (uint32 j = 0; j < 3; ++j)
        {
            int& currentIndex = indices[i].i[j];
            currentIndex = vertexRedirection[currentIndex];
        }

        delete[] vertexRedirection;
    }

    // entry-point of optimization-procedures.
    void
    Optimizer::OptimizeScene(Scene* sceneToOptimize) const
    {
        std::cout << "Optimizing scene" << std::endl;
        for (uint32 i = 0; i < sceneToOptimize->meshes.size(); ++i)
        {
            optimizeMesh(&sceneToOptimize->meshes[i]);
        }
    }
} // namespace GPTranslation
