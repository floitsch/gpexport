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

#include "ms3dExporter.hpp"
#include "plugin.hpp"
#include <maya/MFnPlugin.h>

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
	void*
	creator()
	    {
		return new MS3DPlugin();
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
	void
	Export(const IExporterConfig& config)
	    {
		// MS3DExporter::Export(config); // can't do :(
		MS3DExporter exporter;
		exporter.Export(config);
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
