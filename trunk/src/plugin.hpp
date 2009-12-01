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

/*
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
	virtual void Export(const IExporterConfig& config) = 0;

	/**
	 * returns the extension of this exporter.
	 */
	virtual std::string getExtension() const = 0;
    };
} // namespace GPTranslation
#endif // include guard
