#include "executable.hpp"
#include <maya/MFileIO.h>
#include <maya/MFileObject.h>

#include "exporterConfig.hpp"
#include "ms3dExporter.hpp"

void
printUsage(const std::string& progName)
{
    std::cout << "usage: " << progName << " maya-file exported-file" << std::endl;
}

int
main(int argc, char** argv)
{
    try
    {
		if (argc != 3)
		{
			printUsage(argv[0]);
			return -1;
		}

		const std::string  outFile(argv[2]);

		MFileObject outMFile;
		outMFile.setFullName(outFile.c_str());
		GPTranslation::ExporterConfig config(outMFile, "", false);
		MS3DTranslation::MS3DExporter exporter(&config);

		GPTranslation::exportScene(argv[0], exporter, argv[1]);

		return 0;
    }
    catch (...)
    {
		return -1;
    }
}
