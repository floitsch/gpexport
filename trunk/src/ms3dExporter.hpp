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
 * MS3D-Exporter. Complete exporter that combines several sub-classes.
 * This class itself is mostly "configuring" the sub-classes (implementing the
 * template-methods).
 */

#ifndef ___HPP_MS3D_EXPORTER
#define ___HPP_MS3D_EXPORTER

#include "ms3dWriter.hpp"
#include "gpExporter.hpp"
#include "iExporterConfig.hpp"

namespace MS3DTranslation
{
    class MS3DExporter
	: virtual protected GPBase,
	  public GPExporter,
	  protected MS3DWriter
    {
    public:
	void Export(const IExporterConfig& config)
	    { GPExporter::Export(config); }

    protected:
	void WriteScene(const Scene& extractedScene,
			const std::string& outFile)
	    { MS3DWriter::WriteScene(extractedScene, outFile); }
    };

} // namespace GPTranslation
#endif // include guard
