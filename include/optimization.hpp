/*
 * 2004/06/27
 * Florian Loitsch
 *
 * optimizes the scene (merge duplicated vertices...)
 */

#ifndef ___HPP_GPEXPORT_OPTIMIZATION
#define ___HPP_GPEXPORT_OPTIMIZATION

#include <maya/MDagPath.h>

#include "gpBase.hpp"
#include "scene.hpp"
#include "mesh.hpp"

namespace GPTranslation
{
    class Optimizer
	: protected virtual GPBase
    {
    public:
	/**
	 * optimizes the scene (removes duplicates...)
	 */
	virtual void OptimizeScene(Scene* sceneToOptimize) const;
	static void optimizeSubMesh(const Mesh& mayaMesh, const size_t si, const size_t ei,
		std::vector<Vertex>& newVertices, std::vector<Int3>& newIndices);

    private:
		void optimizeMesh(Mesh* meshToOptimize) const;
		void optimizeMesh_my(Mesh &mesh) const;
    };
} // namespace GPTranslation
#endif // include guard
