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
