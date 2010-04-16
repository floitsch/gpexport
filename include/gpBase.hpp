/*
 * 2004/06/24
 * Florian Loitsch
 *
 * base-class for all GPExport-classes.
 */

#ifndef ___HPP_GPEXPORT_BASE
#define ___HPP_GPEXPORT_BASE

#include <maya/MStatus.h>

#include <string>
#include <iostream>

namespace GPTranslation
{
    class GPBase
    {
    protected:
	static void error(const std::string& errorMessage)
            {
                cout << errorMessage << endl;
                throw errorMessage; // TODO: throw real exception
            }

	void checkStatus(const std::string& errorMessage) const
            {
                if (!status)
                {
                    error("Status is not ok: " + errorMessage);
                }
            }

	mutable MStatus status; // used for all Maya-functions.
    };

} // namespace GPTranslation
#endif // include guard
