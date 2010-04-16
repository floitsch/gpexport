/*
 * 2004/06/27
 * Florian Loitsch
 *
 * configures the base-exporter.
 */

#ifndef ___HPP_GPEXPORT_IEXPORTER_CONFIG
#define ___HPP_GPEXPORT_IEXPORTER_CONFIG

#include <string>

namespace GPTranslation
{
    class IExporterConfig
    {
    public:
	virtual ~IExporterConfig() {}
	virtual std::string getOutFile() const = 0;
	virtual bool doExportSelected() const = 0;
	virtual bool doExportMeshes() const = 0;
	virtual bool doExportPositions() const = 0;
	virtual bool doExportNormals() const = 0;
	virtual bool doExportUVs() const = 0;
	virtual bool doExportWeights() const = 0;
	virtual bool doExportMaterials() const = 0;
	virtual bool doExportSkeleton() const = 0;
	virtual bool doExportAnimation() const = 0;
    };
} // namespace GPTranslation
#endif // include guard
