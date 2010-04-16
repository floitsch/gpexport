/*
 * 2004/06/29
 * Florian Loitsch
 *
 * Plugin-specific methods.
 */

#ifndef ___WOOT_HPP_PLUGIN
#define ___WOOT_HPP_PLUGIN

#include <maya/MPxFileTranslator.h>
#include <string>
#include "iExporterConfig.hpp"

namespace GPTranslation
{
    class Plugin
	: public MPxFileTranslator
    {
    public: // don't touch these methods (or at least: you don't need to).
        bool haveReadMethod() const;
        MStatus reader(const MFileObject& file,
		       const MString& optionsString,
		       FileAccessMode mode);
        bool haveWriteMethod() const;

	// calls the virtual Export()-function.
        MStatus writer(const MFileObject& file,
                       const MString& optionsString,
                       FileAccessMode mode);

		virtual IExporterConfig* newConfig(const MFileObject& file, const MString& optionsString, bool bExportSelected) = 0;
		void deleteConfig(IExporterConfig** config);

	MString defaultExtension() const;

    public: // override if you want.
	/**
	 * rudimentary identifyFile-method. just checks, if the extension is the right one.
	 * Extend/Replace if you want.
	 */
        MFileKind identifyFile(const MFileObject& filename,
                               const char* buffer,
                               short size) const;

    protected:
	/**
	 * does the actual work.
	 */
	virtual void Export() = 0;

	/**
	 * returns the extension of this exporter.
	 */
	virtual std::string getExtension() const = 0;

	IExporterConfig* m_config;
    };
} // namespace GPTranslation
#endif // include guard
