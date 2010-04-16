/*
 * 2004/06/29
 * Florian Loitsch
 *
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
	  public GPExporter
	  //protected MS3DWriter
    {
	public:
		MS3DExporter( const IExporterConfig *config )
			: GPExporter(config), m_writer(config)
		{
			//m_exporterConfig = &config;
		}
    public:
	void Export()
	    { GPExporter::Export(); }

    protected:
	void WriteScene(const Scene& extractedScene)
	    { m_writer.WriteScene(extractedScene); }

	protected:
		MS3DWriter m_writer;
    };

} // namespace GPTranslation
#endif // include guard
