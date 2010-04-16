/*
 * 2004/06/27
 * Florian Loitsch
 *
 * data-structures Skeletons.
 */

#ifndef ___HPP_GPEXPORT_SKELETON
#define ___HPP_GPEXPORT_SKELETON

#include <maya/MMatrix.h>

#include <vector>
#include <string>

namespace GPTranslation
{
    struct JointNode
    {
        std::string             name;
        std::vector<JointNode>  children;
        MMatrix                 localTransform;

	// is not necessary (as we have the hierarchy,
	// and the localTransform).
	// only included to avoid recalculations.
	MMatrix                 worldTransform;
    };

    struct Skeleton
    {
        std::vector<JointNode> rootNodes;
    };
} // namespace GPTranslation
#endif // include guard
