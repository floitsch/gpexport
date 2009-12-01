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
 * a nearly complete exporter. 
 */

#ifndef ___HPP_GPEXPORTER
#define ___HPP_GPEXPORTER

#include <maya/MDagPath.h>

#include <string>

#include "gpBase.hpp"
#include "sceneExtraction.hpp"
#include "iExporterConfig.hpp"
#include "optimization.hpp"

namespace GPTranslation
{
    class GPExporter
	: protected virtual GPBase,
	  protected SceneExtractor,
	  protected Optimizer
    {
    public:
	virtual ~GPExporter() {}

	virtual void Export(const IExporterConfig& exporterConfig);

    protected:
	virtual void WriteScene(const Scene& scene,
				const std::string& fileName) = 0;
	
	virtual bool doExtractMeshes() const;
	virtual bool doExtractSkeleton() const;
	virtual bool doExtractAnimation() const;
	virtual bool doExtractPositions() const;
	virtual bool doExtractWeights() const;
	virtual bool doExtractNormals() const;
	virtual bool doExtractUVs() const;
	virtual bool doExtractMaterials() const;

	// returns true, if a problem occurs.
	virtual bool isVisible(const MDagPath& dagPath) const;

	bool isObjectOrParentSelected(const MDagPath& dagPath) const;
	
	// returns true, if the given dagPath should be exported
	// to decide, it looks (well. actually "should look") at the
	// option-struct.
	virtual bool shouldMeshBeExtracted(const MDagPath& dagPath) const;

	// returns true, if the given joint should be exported.
	virtual bool shouldJointBeExtracted(const MDagPath& dagPath) const;

    private:
	const IExporterConfig* m_exporterConfig;
    };
} // namespace GPTranslation
#endif // include guard
