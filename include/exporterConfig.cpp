#include "exporterConfig.hpp"
#include <maya/MStringArray.h>
#include "basic_types.hpp"
#include "gplog.h"

namespace GPTranslation
{
    // parses the given options, and puts them into the member-var
    //
    // if you are clever, you use this for both. the plugin and
    // the stand-alone.
    ExporterConfig::ExporterConfig(const MFileObject& file,
				   const MString& optionsString,
				   bool  bExportSelected)
    {
	_GPDEBUG("optionsString="<<optionsString );
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
	_GPLOG("file name: "              << m_outFile                );
	_GPLOG("exportSelected: "         << m_doExportSelected       );
	_GPLOG("exportMeshes: "           << m_doExportMeshes         );
	_GPLOG("exportPositions: "        << m_doExportPositions      );
	_GPLOG("exportNormals: "          << m_doExportNormals        );
	_GPLOG("exportUVs: "              << m_doExportUVs            );
	_GPLOG("exportWeights: "          << m_doExportWeights        );
	_GPLOG("exportSkeleton: "         << m_doExportSkeleton       );
	_GPLOG("exportAnimation: "        << m_doExportAnimation      );
    }
} // namespace GPTranslation
