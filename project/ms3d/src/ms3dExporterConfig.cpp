#include "ms3dExporterConfig.hpp"
#include <maya/MStringArray.h>
#include "basic_types.hpp"

namespace GPTranslation
{
    // parses the given options, and puts them into the member-var
    //
    // if you are clever, you use this for both. the plugin and
    // the stand-alone.
    ms3dExporterConfig::ms3dExporterConfig(const MFileObject& file,
				   const MString& optionsString,
				   bool  bExportSelected)
    {
	std::cout << "optionsString="<<optionsString << std::endl;
	m_outFile = file.fullName().asChar();
	
	m_doExportSelected = bExportSelected;
	// currently just hardcoded:
	m_doExportMeshes    = false;
	m_doExportPositions = true;
	m_doExportNormals   = true;
	m_doExportUVs       = true;
	m_doExportWeights   = true;
	m_doExportMaterials = true;
	m_doExportSkeleton  = true;
	m_doExportAnimation = true;
	
	MStringArray options;
	optionsString.split(';', options);
	for (uint32 i = 0; i < options.length(); ++i)
	{
	    MStringArray option;
	    options[i].split('=', option);
	    if (option.length() < 2)
		continue;
	    
	    if (option[0] == "exportMesh")
		m_doExportMeshes = (option[1] == "true");
	    else if (option[0] == "exportSkeleton")
		m_doExportSkeleton = (option[1] == "true");
	    else if (option[0] == "exportAnimation")
		m_doExportAnimation = (option[1] == "true");
	}
	
	// TODO: should be done over user-interface
	m_doExportWeights = m_doExportSkeleton;
	
	// print options:
	std::cout << "filename: "               << m_outFile                << std::endl;
	std::cout << "exportSelected: "         << m_doExportSelected       << std::endl;
	std::cout << "exportMeshes: "           << m_doExportMeshes         << std::endl;
	std::cout << "exportPositions: "        << m_doExportPositions        << std::endl;
	std::cout << "exportNormals: "          << m_doExportNormals          << std::endl;
	std::cout << "exportUVs: "              << m_doExportUVs              << std::endl;
	std::cout << "exportWeights: "          << m_doExportWeights          << std::endl;
	std::cout << "exportSkeleton: "         << m_doExportSkeleton         << std::endl;
	std::cout << "exportAnimation: "        << m_doExportAnimation        << std::endl;
    }
} // namespace GPTranslation
