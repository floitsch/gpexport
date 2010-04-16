/*
 * 2004/06/27
 * Florian Loitsch
 *
 * data-structures for scenes
 */

#ifndef ___HPP_GPEXPORT_SCENE
#define ___HPP_GPEXPORT_SCENE

#include <vector>

#include "mesh.hpp"
#include "animation.hpp"
#include "skeleton.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    struct Scene
    {
        std::vector<Mesh> meshes; // == models
        Skeleton          skeleton;
        Animation         animation;
    };
} // namespace GPTranslation
#endif // include guard
