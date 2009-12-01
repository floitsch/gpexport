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

#include "exporterConfig.hpp"
#include <maya/MStringArray.h>
#include "basic_types.hpp"

namespace GPTranslation
{
    // parses the given options, and puts them into the member-var
    //
    // if you are clever, you use this for both. the plugin and
    // the stand-alone.
    ExporterConfig::ExporterConfig(const MFileObject& file,
				   const MString& optionsString,
				   bool  bExportSelected)
    {
	std::cout << optionsString << std::endl;
	m_outFile = file.fullName().asChar();
	
	m_doExportSelected = bExportSelected;
	// currently just hardcoded:
	m_doExportMeshes    = false;
	m_doExportPositions = true;
	m_doExportNormals   = true;
	m_doExportUVs       = true;
	m_doExportWeights   = true;
	m_doExportMaterials = true;
	m_doExportSkeleton  = true;
	m_doExportAnimation = true;
	
	MStringArray options;
	optionsString.split(';', options);
	for (uint32 i = 0; i < options.length(); ++i)
	{
	    MStringArray option;
	    options[i].split('=', option);
	    if (option.length() < 2)
		continue;
	    
	    if (option[0] == "exportMesh")
		m_doExportMeshes = (option[1] == "true");
	    else if (option[0] == "exportSkeleton")
		m_doExportSkeleton = (option[1] == "true");
	    else if (option[0] == "exportAnimation")
		m_doExportAnimation = (option[1] == "true");
	}
	
	// TODO: should be done over user-interface
	m_doExportWeights = m_doExportSkeleton;
	
	// print options:
	std::cout << "filename: "               << m_outFile                << std::endl;
	std::cout << "exportSelected: "         << m_doExportSelected       << std::endl;
	std::cout << "exportMeshes: "           << m_doExportMeshes         << std::endl;
	std::cout << "exportPositions: "        << m_doExportPositions        << std::endl;
	std::cout << "exportNormals: "          << m_doExportNormals          << std::endl;
	std::cout << "exportUVs: "              << m_doExportUVs              << std::endl;
	std::cout << "exportWeights: "          << m_doExportWeights          << std::endl;
	std::cout << "exportSkeleton: "         << m_doExportSkeleton         << std::endl;
	std::cout << "exportAnimation: "        << m_doExportAnimation        << std::endl;
    }
} // namespace GPTranslation
