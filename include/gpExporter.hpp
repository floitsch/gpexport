/*
 * 2004/06/28
 * Florian Loitsch
 *
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
	: 
	public virtual GPBase,//protected virtual GPBase,
	public SceneExtractor,//protected SceneExtractor,
	public Optimizer //protected Optimizer
    {
    public:
		GPExporter(const IExporterConfig* config) 
			:m_exporterConfig(config)
	{}
	virtual ~GPExporter() {}

	//virtual void Export(const IExporterConfig& exporterConfig);
	virtual void Export() = 0;

    protected:
	//virtual void WriteScene(const Scene& scene, const std::string& fileName) = 0;
	
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

#ifdef NDEBUG
    private:
#else
	protected://  [10/29/2009 yys]
#endif
	const IExporterConfig* m_exporterConfig;
    };
} // namespace GPTranslation
#endif // include guard
