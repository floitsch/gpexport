/*
 * 2004/06/27
 * Florian Loitsch
 *
 * converts the scene into another unit-system, and/or coordonate-system.
 */

#include "conversion.hpp"

#include <maya/MFloatMatrix.h>

namespace GPTranslation
{
    // applies matrix to every point of the mesh.
    void
    Converter::convertMesh(Mesh* pMesh, const MMatrix& targetSystemMatrix) const
    {
        // convert the positions:
        for (unsigned int i = 0; i < pMesh->positions.size(); ++i)
            pMesh->positions[i] = pMesh->positions[i] * targetSystemMatrix;

        // convert the normals:
        MFloatMatrix floatMatrix;
        for (unsigned int i = 0; i < 4; ++i)
            for (unsigned int j = 0; j < 4; ++j)
                floatMatrix[i][j] = (float)targetSystemMatrix[i][j];
        for (unsigned int i = 0; i < pMesh->normals.size(); ++i)
            pMesh->normals[i] = pMesh->normals[i] * floatMatrix;
    }

    // converts every mesh one by one.
    void
    Converter::convertMeshes(std::vector<Mesh>* pMeshes, const MMatrix& targetSystemMatrix) const
    {
        for (unsigned int i = 0; i < pMeshes->size(); ++i)
            convertMesh(&(*pMeshes)[i], targetSystemMatrix);
    }

    // converts every joint-node:
    // two matrices need to be converted (although the world-transformation is still redundant):
    //   - local-transform
    //   - world-transform
    //
    // note, that we need to apply the inverse-matrix (at the appropriate location).
    void
    Converter::convertJointNode(JointNode* pJointNode,
                                   const MMatrix& targetSystemMatrix,
                                   const MMatrix& targetSystemInverseMatrix) const
    {
        pJointNode->localTransform = targetSystemInverseMatrix * pJointNode->localTransform * targetSystemMatrix;
        pJointNode->worldTransform = targetSystemInverseMatrix * pJointNode->worldTransform * targetSystemMatrix;

        for (unsigned int i = 0; i < pJointNode->children.size(); ++i)
            convertJointNode(&pJointNode->children[i], targetSystemMatrix, targetSystemInverseMatrix);
    }

    // converts the skeleton.
    void
    Converter::convertSkeleton(Skeleton* pSkeleton, const MMatrix& targetSystemMatrix) const
    {
        MMatrix targetSystemInverseMatrix = targetSystemMatrix.inverse();
        for (unsigned int i = 0; i < pSkeleton->rootNodes.size(); ++i)
            convertJointNode(&pSkeleton->rootNodes[i], targetSystemMatrix, targetSystemInverseMatrix);
    }

    // converts the keyframe-matrices.
    // as keyframes are just transformations at certain times, they are treated exactly the same, as the
    // transformations in the joint-nodes. (therefore the same multiplication with the inverse-matrix).
    void
    Converter::convertKeyFrame(KeyFrame* pKeyFrame,
                                  const MMatrix& targetSystemMatrix,
                                  const MMatrix& targetSystemInverseMatrix) const
    {
        pKeyFrame->transformation = targetSystemInverseMatrix * pKeyFrame->transformation * targetSystemMatrix;
    }

    // converts every keyframe of the track.
    void
    Converter::convertTrack(Track* pTrack,
                               const MMatrix& targetSystemMatrix,
                               const MMatrix& targetSystemInverseMatrix) const
    {
	for (Track::iterator it = pTrack->begin();
	     it != pTrack->end();
	     ++it)
            convertKeyFrame(&(*it), targetSystemMatrix, targetSystemInverseMatrix);
    }

    // converts every track.
    void
    Converter::convertAnimation(Animation* pAnimation, const MMatrix& targetSystemMatrix) const
    {
        MMatrix targetSystemInverseMatrix = targetSystemMatrix.inverse();
	for (StringTrackMap::iterator it = pAnimation->tracks.begin();
	     it != pAnimation->tracks.end();
	     ++it)
	    convertTrack(&(it->second), targetSystemMatrix, targetSystemInverseMatrix);
    }

    // converts the
    //    - meshes
    //    - skeleton
    //    - animation
    void
    Converter::convertToTargetSystem(Scene* sceneToConvert) const
    {
        const MMatrix targetSystemMatrix = getTargetSystemMatrix();

        if (targetSystemMatrix == MMatrix::identity)
            return;

        convertMeshes   (&sceneToConvert->meshes,    targetSystemMatrix);
        convertSkeleton (&sceneToConvert->skeleton,  targetSystemMatrix);
        convertAnimation(&sceneToConvert->animation, targetSystemMatrix);
    }

    // just forwards.
    // if you want to convert to other units, don't add other methods here,
    // but modify the target-system-matrix.
    // but here's the right place to add another method to convert time-units.
    void
    Converter::ConvertScene(Scene* sceneToConvert) const
    {
        convertToTargetSystem(sceneToConvert);
    }
} // namespace GPTranslation
