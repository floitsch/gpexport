/*
 * Base-Class for FILE*-exporting.
 */

#include "fileWriter.hpp"
#include <stdio.h>
#include <string.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>

//#include "ms3d.h"

namespace GPTranslation
{
    /**
     * opens/closes the file, and delegates the real work to the
     * template-method "doWriteScene"
     */
    void
    FileWriter::WriteScene(const Scene& sceneToWrite,
			   const std::string& fileName)
    {
	std::cout << "Writing ms3d to " << fileName << std::endl;
	m_out = fopen(fileName.c_str(), "wb");
	
	if (m_out == NULL)
	{
	    std::cout << " out == NULL" << std::endl;
	    throw "couldn't open file";
	}
	
	try
	{
	    doWriteScene(sceneToWrite);
	} catch (...) {
	    fclose(m_out);
	    throw;
	}
	fclose(m_out);
    }
}
