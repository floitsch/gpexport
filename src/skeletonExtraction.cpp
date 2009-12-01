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
 * Extracts the skeleton of Maya. Quite straightforward.
 */

#include "skeletonExtraction.hpp"

#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>

namespace GPTranslation
{

    // the depth-parameter is only used for logging
    // Note, that this method assumes, that we are in BindPose.
    JointNode
    SkeletonExtractor::extractSkeletonHierarchy(const MDagPath jointPath,
						uint32 depth) const
    {
        for (uint32 i = 0; i < depth; ++i)
            cout << " ";
        cout << jointPath.partialPathName().asChar() << endl;

        JointNode currentJointNode;
        currentJointNode.name = jointPath.partialPathName().asChar();

        MFnTransform transform(jointPath);
        currentJointNode.localTransform = transform.transformation().asMatrix();
        currentJointNode.worldTransform = jointPath.inclusiveMatrix();
        for (uint32 i = 0; i < jointPath.childCount(); ++i)
        {
            if (jointPath.child(i).hasFn(MFn::kJoint))
            {
                MDagPath childPath;
                childPath.set(jointPath);
                childPath.push(jointPath.child(i));
                if (!shouldJointBeExtracted(childPath))
                    continue; // prune tree
                JointNode childJointNode = extractSkeletonHierarchy(childPath,
								    depth + 1);
                currentJointNode.children.push_back(childJointNode);
            }
            else
            {
                for (uint32 j = 0; j <= depth; ++j)
                    cout << " ";
                cout << "unusual joint-child "
		     << jointPath.partialPathName().asChar()
		     << ".child(" << i << ") ignored" << std::endl;
            }
        }
        return currentJointNode;
    }

    // extracts the skeleton. Note, that there might be several root-nodes.
    // furthermore: if there are references (two joints pointing to the same
    // node), they will be "separated" now (ie. there will be 2 different
    // entries).
    Skeleton
    SkeletonExtractor::ExtractSkeleton() const
    {
        cout << std::endl << " Extracting Skeleton" << std::endl;
        Skeleton skeleton;
        for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
             !dagIt.isDone();
             dagIt.next())
        {
            MDagPath jointPath;
            dagIt.getPath(jointPath);
            if (shouldJointBeExtracted(jointPath))
                skeleton.rootNodes.push_back(
		    extractSkeletonHierarchy(jointPath, 2));
	    // children have already been treated by extractSkeletonHierarchy,
	    // or are intentionally ignored
            dagIt.prune();
        }
        return skeleton;
    }

    
} // namespace GPTranslation
