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
 * The necessary functions, to make this exporter "self-contained" (in
 * other words: an executable). Advantage: it can be used in shell-scripts.
 */

#include <maya/MLibrary.h>
#include <maya/MFileIO.h>
#include <maya/MFileObject.h>

#include "exporterConfig.hpp"
#include "executable.hpp"

#include <iostream>

namespace GPTranslation
{
    class MayaLibrary
    {
    public:
	MayaLibrary(char* appName)
	    {
		bool status = MLibrary::initialize(appName, false);
		if (!status)
		    throw "couldn't initialize library";
	    }

	~MayaLibrary()
	    {
		MLibrary::cleanup();
	    }
    };

    void checkStatus(MStatus status, const std::string& errorMessage)
    {
	if (!status)
	    throw "checkStatus failed: " + errorMessage;
    }

    void loadScene(const std::string& sceneFileName)
    {
	MStatus status;
	status = MFileIO::newFile(true);
	checkStatus(status, "newFile failed");

	std::cout << "Opening " << sceneFileName << std::endl;
	status = MFileIO::open(sceneFileName.c_str());
	checkStatus(status, "opening " + sceneFileName + " failed");
    }

    void exportScene(const std::string& exporterName,
		     GPExporter& exporter,
		     const std::string& inFile,
		     const std::string& outFile)
    {
	MayaLibrary library((char*)exporterName.c_str());

	loadScene(inFile);

	MFileObject outMFile;
	outMFile.setFullName(outFile.c_str());
	ExporterConfig config(outMFile, "", false);

	exporter.Export(config);
    }
} // namespace GPTranslation
