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
 * extracts Materials out of MayaMesh.
 */

#ifndef ___HPP_GPEXPORT_MATERIAL_EXTRACTION
#define ___HPP_GPEXPORT_MATERIAL_EXTRACTION

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>

#include <vector>
#include <string>

#include "gpBase.hpp"
#include "basic_types.hpp"
#include "material.hpp"

namespace GPTranslation
{
    class MaterialExtractor
	: public virtual GPBase
    {
    public:
	/**
	 * Extracts all materials of the given dagpath.
	 * These methods aren't very well tested, but should be fine.
	 * most of the code is inspired by Ogre's Maya-exporter.
	 */
	std::vector<Material> ExtractMaterials(const MDagPath& mayaMeshPath) const;
    private:
	std::vector<std::string> extractTextures(MPlug& shaderPlugSource) const;
	template<typename T>
	double getShininess(const T& shader) const;
	template<typename T>
	void fillMaterial(Material* pmaterial, const MObject& shaderNode, const std::string& shadingType) const;
	std::vector<Material> extractMaterials(const MObject& shaderSet) const;
    };
} // namespace GPTranslation
#endif // include guard
