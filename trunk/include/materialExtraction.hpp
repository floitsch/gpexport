/*
 * 2004/06/19
 * Florian Loitsch
 *
 * extracts Materials out of MayaMesh.
 */

#ifndef ___HPP_GPEXPORT_MATERIAL_EXTRACTION
#define ___HPP_GPEXPORT_MATERIAL_EXTRACTION

#include <vector>
#include <string>

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>


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
	MColor getSpecular(const T& shader) const;
// 	template<typename T>
// 	void fillMaterial(Material* pmaterial, const MObject& shaderNode, const std::string& shadingType) const;
	template<typename T>
	void fillMaterial2(Material &material, const MObject& shaderNode, const std::string& shadingType) const;
	
	std::vector<Material> extractMaterials(const MObject& shaderSet) const;
	void extractTextureName(const MObject& shaderSet)const;
	std::string getTableNameOfShaderPlugSource(const MPlug &plugsource)const;
	bool OutputColor(MFnDependencyNode& fn,const char* name) const;
	bool OutputColor2(MFnDependencyNode& fn,const char* name, GPTranslation::Channel &channel) const;
	void OutputBumpMap(MFnDependencyNode& fn, GPTranslation::Channel &bump) const;
	};
} // namespace GPTranslation
#endif // include guard
