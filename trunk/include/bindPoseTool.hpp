/*
 * 2004/06/24
 * Florian Loitsch
 *
 * tool, to move the skeleton easily into bindpose (and undo this transformation).
 */

#ifndef ___HPP_GPEXPORT_BINDPOSE_TOOL
#define ___HPP_GPEXPORT_BINDPOSE_TOOL

#include <maya/MObject.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnIkJoint.h>

#include <vector>

#include "gpBase.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    struct BindPoseUndoInformation
    {
        bool ikSnap;
        bool ikSolve;

        typedef std::vector<std::pair<MObject, MTransformationMatrix> > JointMatrixVector;
        JointMatrixVector savedTransforms; // MObject should always be Joint
    };

    
    class BindPoseTool
	: public virtual GPBase
    {
    public:
	BindPoseTool();

	/**
	 * if the skeleton is not in bindpose, the destructor will reset it.
	 * This is useful when using exceptions.
	 */
	~BindPoseTool();

	/**
	 * puts the skeleton into its bindpose.
	 */
	void GoIntoBindPose();
	/**
	 * Undoes the transformations done by "GoIntoBindPose()".
	 */
	void UndoGoIntoBindPose();

    private: // methods
	MMatrix getBindPoseMatrix(MFnIkJoint joint) const;
	void syncMeshes() const;

    private: // members
	BindPoseUndoInformation undoInfo;
	bool inBindPose;
    };
} // namespace GPTranslation
#endif // include guard
