/*
 * 2004/06/27
 * Florian Loitsch
 *
 * Extracts the whole scene.
 */

#include "bindPoseTool.hpp"
#include "sceneExtraction.hpp"
#include "gplog.h"

namespace GPTranslation
{
    // extracts the complete scene (depending on the options),
    Scene
    SceneExtractor::ExtractScene() const
    {
        _GPLOG( "Extracting scene" );
        Scene extractedScene;
        if (doExtractMeshes() || doExtractSkeleton())
        {
			BindPoseTool bindPoseTool;
			BindPoseUndoInformation bindPoseUndoInformation;
			if (doExtractWeights() || doExtractSkeleton())
				bindPoseTool.GoIntoBindPose();

			extractedScene.meshes = ExtractMeshes();
			if (doExtractSkeleton())
				extractedScene.skeleton = ExtractSkeleton();

				// TODO: this is not yet exception-save!
			if (doExtractWeights() || doExtractSkeleton())
				bindPoseTool.UndoGoIntoBindPose();
        }

        if (doExtractAnimation())
        {
            extractedScene.animation = ExtractAnimation();
        }
        return extractedScene;
    }
} // namespace GPTranslation
