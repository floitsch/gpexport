/*
 * 2004/06/27
 * Florian Loitsch
 *
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
