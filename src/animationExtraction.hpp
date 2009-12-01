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
