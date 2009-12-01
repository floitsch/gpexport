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

#include "plugin.hpp"
#include "exporterConfig.hpp"

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
	    bool bExportSelected = (mode == MPxFileTranslator::kExportActiveAccessMode);
	    ExporterConfig config(file, optionsString, bExportSelected);
	    Export(config);
	    return MS::kSuccess;
	}
	catch (...)
	{
	    return MS::kFailure;
	}
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
