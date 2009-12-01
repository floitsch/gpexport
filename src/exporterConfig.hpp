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
 * Editor-config-class. parses the options, and stores them.
 */

#ifndef ___HPP_GPEXPORT_EXPORTER_CONFIG
#define ___HPP_GPEXPORT_EXPORTER_CONFIG

#include <maya/MFileObject.h>
#include <maya/MString.h>
#include <string>

#include "iExporterConfig.hpp"

namespace GPTranslation
{
    struct ExporterConfig
	: public IExporterConfig
    {
    public:
	ExporterConfig(const MFileObject& file,
		       const MString& optionsString,
		       bool  bExportSelected);

	std::string getOutFile() const
	    { return m_outFile; }
	
	bool doExportSelected() const
	    { return m_doExportSelected; }
	
	bool doExportMeshes() const
	    { return m_doExportMeshes; }
	
	bool doExportPositions() const
	    { return m_doExportPositions; }
	
	bool doExportNormals() const
	    { return m_doExportNormals; }
	
	bool doExportUVs() const
	    { return m_doExportUVs; }
	
	bool doExportWeights() const
	    { return m_doExportWeights; }
	
	bool doExportMaterials() const
	    { return m_doExportMaterials; }
	
	bool doExportSkeleton() const
	    { return m_doExportSkeleton; }
	
	bool doExportAnimation() const
	    { return m_doExportAnimation; }

    private:
        std::string m_outFile;
        bool        m_doExportSelected;
        bool        m_doExportMeshes;
        bool        m_doExportPositions;
        bool        m_doExportNormals;
        bool        m_doExportUVs;
        bool        m_doExportWeights;
	bool        m_doExportMaterials;
        bool        m_doExportSkeleton;
        bool        m_doExportAnimation;
    };
} // namespace GPTranslation
#endif // include guard
