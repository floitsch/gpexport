/*
 * 2004/06/24
 * Florian Loitsch
 * 
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
