/*
 * 2004/07/04
 * Florian Loitsch
 *
 * Executable-specific functions.
 */

#include <string>
#include "gpExporter.hpp"

namespace GPTranslation
{
    void exportScene(const std::string& exporterName,
		     GPExporter &exporter,
		     const std::string& inFile);
} // namespace GPTranslation
