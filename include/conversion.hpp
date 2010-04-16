/*
 * 2004/06/27
 * Florian Loitsch
 *
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
