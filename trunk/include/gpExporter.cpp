/*
 * mostly implements all template-methods (with more or less useful defaults).
 */

#include "gpExporter.hpp"
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>

namespace GPTranslation
{
    bool
    GPExporter::doExtractMeshes() const
    {
	return m_exporterConfig->doExportMeshes();
    }

    bool
    GPExporter::doExtractSkeleton() const
    {
	return m_exporterConfig->doExportSkeleton();
    }
    
    bool
    GPExporter::doExtractAnimation() const
    {
	return m_exporterConfig->doExportAnimation();
    }
    

    bool
    GPExporter::doExtractPositions() const
    {
	return m_exporterConfig->doExportPositions();
    }
    
    bool
    GPExporter::doExtractWeights() const
    {
	return m_exporterConfig->doExportWeights();
    }

    bool
    GPExporter::doExtractNormals() const
    {
	return m_exporterConfig->doExportNormals();
    }

    bool
    GPExporter::doExtractUVs() const
    {
	return m_exporterConfig->doExportUVs();
    }

    bool
    GPExporter::doExtractMaterials() const
    {
	return m_exporterConfig->doExportMaterials();
    }

    // returns true, if a problem occurs.
    // normally i would work with exceptions here. But unfortunately exceptions don't work here. (compiler-bug?)
    bool
    GPExporter::isVisible(const MDagPath& dagPath) const
    {
	bool isVisible;
	MFnDagNode dagNode(dagPath.node());
	if (dagNode.isIntermediateObject()) return false;
	
	MPlug visibilityPlug = dagNode.findPlug("visibility", &status);
	if (status) {
	    visibilityPlug.getValue(isVisible);
	    if (!isVisible) return false;
	}
	
	MPlug lodVisibilityPlug = dagNode.findPlug("lodVisibility", &status);
	if (status)
	{
	    lodVisibilityPlug.getValue(isVisible);
	    if (!isVisible) return false;
	}
	
	MPlug overrideEnablePlug = dagNode.findPlug("overrideEnabled", &status);
	if (status)
	{
	    bool isOverridden;
	    overrideEnablePlug.getValue(isOverridden);
	    if (isOverridden)
	    {
		MPlug overrideVisibilityPlug = dagNode.findPlug("overrideVisibility", &status);
		if (status)
		{
		    overrideVisibilityPlug.getValue(isVisible);
		    if (!isVisible) return false;
		}
	    }
	}
	
	return true;
    }

    bool
    GPExporter::isObjectOrParentSelected(const MDagPath& dagPath) const
    {
	MSelectionList selectedElements;
	MGlobal::getActiveSelectionList(selectedElements);

	MDagPath toSearchPath(dagPath);
	while (toSearchPath.length() > 0)
	{
	    // look, if the object is the selection-list
	    MItSelectionList iter(selectedElements, MFn::kDagNode);
	    while (!iter.isDone())
	    {
		MDagPath tmpPath;
		if (iter.getDagPath(tmpPath))
		    if (tmpPath == toSearchPath)
			return true;

		iter.next();
	    }
	    toSearchPath.pop();
	}
	return false;
    }
    
    // returns true, if the given dagPath should be exported
    // to decide, it looks (well. actually "should look") at the
    // option-struct.
    bool
    GPExporter::shouldMeshBeExtracted(const MDagPath& dagPath) const
    {
	// is this a nice mesh?
	if (dagPath.hasFn(MFn::kTransform)) return false;
	if (!dagPath.hasFn(MFn::kMesh)) return false;
	
	if (!isVisible(dagPath)) return false;

	if (m_exporterConfig->doExportSelected() &&
	    !isObjectOrParentSelected(dagPath))
	    return false;

	return true;
    }

    // returns true, if the given joint should be exported.
    bool
    GPExporter::shouldJointBeExtracted(const MDagPath& dagPath) const
    {
	// is this a nice joint? (sanity check)
	if (!dagPath.hasFn(MFn::kJoint)) return false;
	
	if (!isVisible(dagPath)) return false;

    	if (m_exporterConfig->doExportSelected() &&
	    !isObjectOrParentSelected(dagPath))
	    return false;

	return true;
    }

    void
    GPExporter::Export()
    {
		cout<< "  --> GPExporter::Export()" << endl;
// 	//m_exporterConfig = &config;
// 	std::cout << "=================================================" << std::endl;
// 	std::cout << "Starting Export" << std::endl;
// 	Scene extractedScene = ExtractScene();
// 	std::cout << std::endl;
// 	OptimizeScene(&extractedScene);
// 	std::cout << std::endl;
// 	WriteScene(extractedScene, m_exporterConfig.getOutFile());
    }
} // namespace GPTranslation
