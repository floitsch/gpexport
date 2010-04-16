/*
 * 2004/06/27
 * Florian Loitsch
 *
 * Optimizes the scene.
 */

#include "optimization.hpp"
#include "basic_types.hpp"
#include "gplog.h"

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
        _GPDEBUG( " optimizing mesh " << meshToOptimize->name );
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
        _GPDEBUG( "  reduced from " << oldVertexCount << " to " << vertexCounter );

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
        _GPDEBUG( "Optimizing scene" );
        for (uint32 i = 0; i < sceneToOptimize->meshes.size(); ++i)
        {
            optimizeMesh(&sceneToOptimize->meshes[i]);
			//optimizeMesh_my(sceneToOptimize->meshes[i]);//  [11/16/2009 yaoyansi]
        }
    }

	void Optimizer::optimizeSubMesh(const Mesh& mayaMesh, const size_t si, const size_t ei,
		std::vector<Vertex>& newVertices, std::vector<Int3>& newIndices)
	{//[si, ei)
		newIndices.resize(ei-si);

		for(int triIndex = si; triIndex<ei; ++triIndex)
		{
			const Int3 &tri = mayaMesh.indices[triIndex];
			for(int j = 0; j<3; ++j)//for each vertex in triangle
			{
				int vI = tri.i[ j ];
				const Vertex &v = mayaMesh.vertices[vI];//original triangle in maya

				std::vector<Vertex>::iterator newvi = std::find( newVertices.begin(), newVertices.end(), v );
				if( newvi == newVertices.end() )
				{
					int newIndex = newVertices.size();
					newVertices.push_back( v );//size++

					newIndices[triIndex - si].i[j] = newIndex;
				}else{
					newIndices[triIndex - si].i[j] = std::distance( newVertices.begin(), newvi );
				}
			}
		}
	}

	//  [11/16/2009 yaoyansi]
	void Optimizer::optimizeMesh_my(Mesh &mesh) const
	{
		std::vector<Vertex>  newVertices;
		std::vector<Int3>  newIndices;

		optimizeSubMesh(mesh, 0, mesh.indices.size(), newVertices, newIndices);
		mesh.vertices = newVertices;
		mesh.indices = newIndices;
	}
} // namespace GPTranslation
