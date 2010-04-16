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
		     GPExporter &exporter,
		     const std::string& inFile)
    {
	MayaLibrary library((char*)exporterName.c_str());
	loadScene(inFile);

	exporter.Export();
    }
} // namespace GPTranslation
