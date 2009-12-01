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
 * Base-Class for FILE*-exporting.
 */

#include "fileWriter.hpp"
#include <stdio.h>
#include <string.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>

#include "ms3d.h"

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
