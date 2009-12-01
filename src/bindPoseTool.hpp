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
