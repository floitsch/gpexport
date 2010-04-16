/*
 * 2004/06/24
 * Florian Loitsch
 *
 * extracts Animation out of Maya
 */

#ifndef ___HPP_GPEXPORT_ANIMATION_EXTRACTION
#define ___HPP_GPEXPORT_ANIMATION_EXTRACTION

#include <maya/MTime.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>

#include <vector>
#include <set>

#include "gpBase.hpp"
#include "basic_types.hpp"
#include "animation.hpp"

namespace GPTranslation
{
    typedef std::multimap<MTime, uint32> Times2JointMap;

    class AnimationExtractor
	: public virtual GPBase
    {
    public:
	/**
	 * extracts the keyframes for all skeletons.
	 * Note, that we don't need the skeleton itself to do this.
	 */
	Animation ExtractAnimation() const;

    private:
	uint32 getFps() const;

	std::set<MTime> extractKeyFrameTimes(const MDagPath& jointPath) const;
	std::vector<MDagPath> getAnimatedJoints() const;

	Times2JointMap getTimes2JointMap(const std::vector<MDagPath>& animatedJointPaths) const;
    };
} // namespace GPTranslation
#endif // include guard
