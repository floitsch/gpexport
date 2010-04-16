#include "ms3dExporter.hpp"
#include "plugin.hpp"
#include <maya/MFnPlugin.h>
#include "ms3dExporterConfig.hpp"

namespace MS3DTranslation
{
    const char* const translatorName = "MS3D_Export";
    const char* const pixmapName = "none";
    const char* const optionsScript = "MS3D_opts";
    const char* const defaultOptions = "";
    const char* const extension = "ms3d";
    const char* const vendor = "OpenSource";
    const char* const version = "0.1";
    const char* const requiredApiVersion = "Any";

    class MS3DPlugin
	: public GPTranslation::Plugin
          //, protected MS3DExporter  // burns Maya... (see "Export()" below
    {
    public:
	// template-method (of GPTranslation::Plugin)
	std::string
	getExtension() const
	    {
		return extension;
	    }

	/**
	 * needed by "initializePlugin"
	 */
	static
		void* creator()
		{
			return new MS3DPlugin();
		}

		IExporterConfig* newConfig(const MFileObject& file, const MString& optionsString, bool bExportSelected)
		{
			return new ms3dExporterConfig(file, optionsString, bExportSelected);
		}
		/**
		 * work-around:
		 *   originally I derived this class from MS3DExporter, and forwarded
		 *   this call to MS3DExporter::Export(config).
		 *   Unfortunately this burned Maya (they reference probably directly
		 *   into the Virtual table, and this doesn't work with the multiple
		 *   inheritance and/or the virtual inheritance...).
		 *   Anyway: this isn't really worse, and it works fine now.
		 */
		void Export()
	    {
			// MS3DExporter::Export(config); // can't do :(
			MS3DExporter exporter(m_config);
			exporter.Export();
	    }
    };
} // namespace MS3DTranslation

MStatus initializePlugin(MObject obj)
{
    using namespace MS3DTranslation;

    MFnPlugin plugin(obj,
		     vendor,
		     version,
		     requiredApiVersion);

    // Register the translator:
    return plugin.registerFileTranslator(translatorName,
					 (char*)pixmapName,
					 MS3DPlugin::creator,
					 (char*)optionsScript,
					 (char*)defaultOptions);
}

MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);
    return plugin.deregisterFileTranslator(MS3DTranslation::translatorName);
}
