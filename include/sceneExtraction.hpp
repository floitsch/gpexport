/*
 * 2004/06/27
 * Florian Loitsch
 *
 * extracts the complete scene
 */

#ifndef ___HPP_GPEXPORT_SCENE_EXTRACTION
#define ___HPP_GPEXPORT_SCENE_EXTRACTION

#include <maya/MDagPath.h>

#include "gpBase.hpp"
#include "scene.hpp"
#include "meshExtraction.hpp"
#include "animationExtraction.hpp"
#include "skeletonExtraction.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    class SceneExtractor
	: protected virtual GPBase,
	  protected MeshExtractor,
	  protected SkeletonExtractor,
	  protected AnimationExtractor
    {
    public:
	/**
	 * extracts the scene of maya. Configuration is done
	 * using template-methods.
	 */
	Scene ExtractScene() const;

    protected:
	/**
	 * returns true, if meshes should be extracted.
	 *
	 * default-implementation returns true.
	 */
 	virtual bool doExtractMeshes() const
	    { return true; }

	/**
	 * returns true, if the skeleton should be extracted.
	 *
	 * default-implementation returns true.
	 */
	virtual bool doExtractSkeleton() const
	    { return true; }

	/**
	 * returns true, if the animation should be extracted.
	 *
	 * default-implementation returns true.
	 */
	virtual bool doExtractAnimation() const
	    { return true; }
    };
} // namespace GPTranslation
#endif // include guard
