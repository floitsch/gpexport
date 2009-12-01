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
 * base-class for a simple file-writer (using FILE*s).
 *
 */

#ifndef ___HPP_GPEXPORT_WRITER
#define ___HPP_GPEXPORT_WRITER

#include <maya/MMatrix.h>

#include <stdio.h>
#include <string.h>

#include "basic_types.hpp"
#include "gpBase.hpp"
#include "scene.hpp"

namespace GPTranslation
{
    class FileWriter
	: virtual protected GPBase
    {
    public:
	/**
	 * opens/closes the file, and delegates the real work to the
	 * template-method "doWriteScene"
	 */
	virtual void WriteScene(const Scene& sceneToWrite,
				const std::string& fileName);

    protected:
	/**
	 * template method that writes the whole scene.
	 * may use the useful methods below (or directly
	 * acces the FILE*-member).
	 */
	virtual void doWriteScene(const Scene& sceneToWrite) = 0;

	// the following methods are quite handy.
	// TODO: error checking...
	template <typename T>
	void fwrite(const T& data)
	    {
		std::fwrite(&data, sizeof(data), 1, m_out);
	    }

	// TODO: error checking...
	template <typename T>
	void fwrite(const T* data, size_t nitems)
	    {
		std::fwrite(data, sizeof(T), nitems, m_out);
	    }

    protected: // members
	FILE* m_out;
    };
} // namespace GPTranslation
#endif // include guard
