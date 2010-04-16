/*
 * 2004/06/24
 * Florian Loitsch
 *
 * extracts Skeletons out of Maya
 */

#ifndef ___HPP_GPEXPORT_SKELETON_EXTRACTION
#define ___HPP_GPEXPORT_SKELETON_EXTRACTION

#include <maya/MDagPath.h>

#include <vector>
#include <string>

#include "gpBase.hpp"
#include "basic_types.hpp"
#include "skeleton.hpp"

namespace GPTranslation
{
    class SkeletonExtractor
	: protected virtual GPBase
    {
    public:
	/**
	 * extracts the skeleton of maya.
	 * This method doesn't change the skeleton's transformation. If the
	 * skeleton should be extracted in its bindpose, this must be done before.
	 */
	Skeleton ExtractSkeleton() const;

    protected:
	/**
	 * template method, that decides, if the given joint should be exported or not.
	 */
	virtual bool shouldJointBeExtracted(const MDagPath& dagPath) const
	    { return true; }

    private:
	JointNode extractSkeletonHierarchy(const MDagPath jointPath, uint32 depth) const;
    };
} // namespace GPTranslation
#endif // include guard
