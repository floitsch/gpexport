/*
 * 2004/06/29
 * Florian Loitsch
 *
 * Plugin-specific methods.
 */

#include "plugin.hpp"
#include <cassert>
#include "exporterConfig.hpp"
#include "gplog.h"

namespace GPTranslation
{
    bool
    Plugin::haveReadMethod() const
    {
	return false;
    }

    MStatus
    Plugin::reader(const MFileObject& file,
		   const MString& optionsString,
		   FileAccessMode mode)
    {
	fprintf(stderr, "GPTranslator::reader called in error\n");
	return MS::kFailure;
    }

    bool
    Plugin::haveWriteMethod() const
    {
	return true;
    }

    // calls the virtual Export()-function.
    MStatus
    Plugin::writer(const MFileObject& file,
		   const MString& optionsString,
		   FileAccessMode mode)
    {
		try
		{
			init_gp_logs("gpExporter.log");
			_GPLOG("\n\n\n\n\n========================================================");

			bool bExportSelected = (mode == MPxFileTranslator::kExportActiveAccessMode);
			
			//ExporterConfig config(file, optionsString, bExportSelected);
			//Export(config);
			m_config = newConfig(file, optionsString, bExportSelected);
			Export();
			deleteConfig(&m_config);
			assert(m_config==NULL);// make sure it is deleted.
			
			close_gp_logs();

			return MS::kSuccess;
		}
		catch (...)
		{
			return MS::kFailure;
		}
    }
	IExporterConfig* Plugin::newConfig(const MFileObject& file, const MString& optionsString, bool bExportSelected)
	{
		return new ExporterConfig(file, optionsString, bExportSelected);
	}
	void Plugin::deleteConfig(IExporterConfig **pConfig)
	{
		delete (*pConfig);
		*pConfig = NULL;
	}

    MString
    Plugin::defaultExtension() const
    {
	return getExtension().c_str();
    }

    /**
     * rudimentary identifyFile-method. just checks, if the extension is the right one.
     * Extend/Replace if you want.
     */
    MPxFileTranslator::MFileKind
    Plugin::identifyFile(const MFileObject& filename,
			 const char* buffer,
			 short size) const
    {
	const char* name = filename.name().asChar();
	int nameLength = strlen(name);
	std::string extension = getExtension();
	int extensionLength = extension.size();
	
	if ((nameLength > extensionLength + 1) &&
	    !strcasecmp(name + nameLength - extensionLength, extension.c_str()))
	    return kCouldBeMyFileType;
	else
	    return kNotMyFileType;
    }
} // namespace GPTranslation
