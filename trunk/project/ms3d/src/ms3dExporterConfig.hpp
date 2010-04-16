/*
 * 2004/06/27
 * Florian Loitsch
 *
 * Editor-config-class. parses the options, and stores them.
 */

#ifndef ___HPP_GPEXPORT_EXPORTER_CONFIG
#define ___HPP_GPEXPORT_EXPORTER_CONFIG

#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <string>

#include "iExporterConfig.hpp"

namespace GPTranslation
{
    struct ms3dExporterConfig
	: public IExporterConfig
    {
    public:
	ms3dExporterConfig(const MFileObject& file,
		       const MString& optionsString,
		       bool  bExportSelected);

	std::string getOutFile() const
	    { return m_outFile; }
	
	bool doExportSelected() const
	    { return m_doExportSelected; }
	
	bool doExportMeshes() const
	    { return m_doExportMeshes; }
	
	bool doExportPositions() const
	    { return m_doExportPositions; }
	
	bool doExportNormals() const
	    { return m_doExportNormals; }
	
	bool doExportUVs() const
	    { return m_doExportUVs; }
	
	bool doExportWeights() const
	    { return m_doExportWeights; }
	
	bool doExportMaterials() const
	    { return m_doExportMaterials; }
	
	bool doExportSkeleton() const
	    { return m_doExportSkeleton; }
	
	bool doExportAnimation() const
	    { return m_doExportAnimation; }

    private:
        std::string m_outFile;
        bool        m_doExportSelected;
        bool        m_doExportMeshes;
        bool        m_doExportPositions;
        bool        m_doExportNormals;
        bool        m_doExportUVs;
        bool        m_doExportWeights;
	bool        m_doExportMaterials;
        bool        m_doExportSkeleton;
        bool        m_doExportAnimation;
    };
} // namespace GPTranslation
#endif // include guard
