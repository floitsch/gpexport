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
 * converts the scene into another unit-system, and/or coordonate-system.
 */

#ifndef ___HPP_GPEXPORT_CONVERTER
#define ___HPP_GPEXPORT_CONVERTER

#include <maya/MMatrix.h>

#include "scene.hpp"
#include "gpBase.hpp"
#include "basic_types.hpp"

namespace GPTranslation
{
    class Converter
	: protected virtual GPBase
    {
    public:
	/**
	 * converts the scene into another Unit-system and/or coordonate-system.
	 */
	void ConvertScene(Scene* sceneToConvert) const;

    protected:
	/**
	 * returns the target-coordinate-system.
	 *
	 * default-implementation returns identity-matrix.
	 */
	virtual MMatrix getTargetSystemMatrix() const
	    { return MMatrix::identity; }

    private:
	void convertMesh(Mesh* pMesh, const MMatrix& targetSystemMatrix) const;
	void convertMeshes(std::vector<Mesh>* pMeshes, const MMatrix& targetSystemMatrix) const;
	void convertJointNode(JointNode* pJointNode,
			      const MMatrix& targetSystemMatrix,
			      const MMatrix& targetSystemInverseMatrix) const;
	void convertSkeleton(Skeleton* pSkeleton, const MMatrix& targetSystemMatrix) const;
	void convertKeyFrame(KeyFrame* pKeyFrame,
			     const MMatrix& targetSystemMatrix,
			     const MMatrix& targetSystemInverseMatrix) const;
	void convertTrack(Track* pTrack,
			  const MMatrix& targetSystemMatrix,
			  const MMatrix& targetSystemInverseMatrix) const;
	void convertAnimation(Animation* pAnimation, const MMatrix& targetSystemMatrix) const;
	void convertToTargetSystem(Scene* sceneToConvert) const;
    };
} // namespace GPTranslation
#endif // include guard
