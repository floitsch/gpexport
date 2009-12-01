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
 * Extracts all Material-related data of Maya.
 */

#include "materialExtraction.hpp"

#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFileObject.h>

#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnLambertShader.h>

#include <maya/MPlugArray.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>

#include <maya/MFnMesh.h>

namespace GPTranslation
{
    /*
     * we extract all textures by looking up the fileTextureName-plug of the
     * shader-plugs.
     * Honestly: I've never used it, and included it just for
     * completeness-sake.
     *
     * the index in the returned vector, should be the index of the uv-set.
     */
    std::vector<std::string>
    MaterialExtractor::extractTextures(MPlug& shaderPlugSource) const
    {
	std::vector<std::string> textures;
	
	// Textures
	MFnDependencyNode shaderDepNode(shaderPlugSource.node());
	
	for (MItDependencyGraph shaderGraphIt(shaderPlugSource,
					      MFn::kFileTexture,
					      MItDependencyGraph::kUpstream);
	     !shaderGraphIt.isDone();
	     shaderGraphIt.next())
	{
	    MObject shaderTextureNode = shaderGraphIt.thisNode();
	    MFnDependencyNode mayaTexture(shaderTextureNode);
	    
	    MString textureFile;
	    mayaTexture.findPlug("fileTextureName").getValue(textureFile);

	    // small trick (seen in X-file exporter) to get the relative path:
	    MFileObject filePath;

	    filePath.setFullName(textureFile);

	    // Note, that the index in the textures-vector represents the
	    // texCoord-index. (uv-index)
	    textures.push_back(filePath.name().asChar());
	}

	return textures;
    }

    /*
     * depending on the Shader, we extract differently the shininess-factor.
     * Using the templates, we limit the copying of code.
     */
    template<typename T>
    double
    MaterialExtractor::getShininess(const T& shader) const
    { throw "not implemented"; }

    template<>
    double
    MaterialExtractor::getShininess<MFnPhongShader>
    (const MFnPhongShader& shader) const
    { return shader.cosPower(); }
    
    template<>
    double
    MaterialExtractor::getShininess<MFnBlinnShader>
    (const MFnBlinnShader& shader) const
    { return shader.specularRollOff(); }
    
    template<>
    double
    MaterialExtractor::getShininess<MFnLambertShader>
    (const MFnLambertShader& shader) const
    { return 0; }

    /*
     * fills the given material.
     */
    template<typename T>
    void
    MaterialExtractor::fillMaterial(Material* pMaterial,
				    const MObject& shaderNode,
				    const std::string& shadingType) const
    {
	T shader(shaderNode);
	std::string shadingMode;
	if (shadingType == "phong")
	    pMaterial->shadingMode = "phong";
	else // blinn and lambert
	    pMaterial->shadingMode = "gouraud";
	    
	pMaterial->ambient = shader.ambientColor();
        pMaterial->diffuse = shader.diffuseCoeff() * shader.color();
	pMaterial->selfIllumination = shader.incandescence();

	pMaterial->shininess = getShininess(shader);
    }

    /*
     * for the given shader-set extracts all materials.
     */
    std::vector<Material>
    MaterialExtractor::extractMaterials(const MObject& shaderSet) const
    {
	std::vector<Material> materials;
	
	MFnDependencyNode depNode(shaderSet);
	MPlug shaderPlug = depNode.findPlug("surfaceShader");
	
	if (shaderPlug.isNull()) return materials; // empty vector
	
	MPlugArray shaderPlugSources;
	shaderPlug.connectedTo(shaderPlugSources, true, false);

	for (uint32 i = 0; i < shaderPlugSources.length(); ++i)
	{
	    Material currentMaterial;

	    MPlug currentShaderPlugSource = shaderPlugSources[i];
	    MObject shaderNode = currentShaderPlugSource.node();
	    if (shaderNode.hasFn(MFn::kPhong))
		fillMaterial<MFnPhongShader>(&currentMaterial,
					     shaderNode,
					     "phong");
	    else if (shaderNode.hasFn(MFn::kBlinn))
		fillMaterial<MFnBlinnShader>(&currentMaterial,
					     shaderNode,
					     "blinn");
	    else if (shaderNode.hasFn(MFn::kLambert))
		fillMaterial<MFnLambertShader>(&currentMaterial,
					       shaderNode,
					       "lambert");
	    else
	    {
		// don't know how to extract it
		continue;
	    }

	    currentMaterial.texturePaths =
		extractTextures(currentShaderPlugSource);

	    materials.push_back(currentMaterial);
	}

	return materials;
    }


    /*
     * for every mesh gets its shader-set, and extracts all materials.
     */
    // I'm heavily referencing Ogre's Material-export...
    // (credits, where credits are due)
    std::vector<Material>
    MaterialExtractor::ExtractMaterials(const MDagPath& mayaMeshPath) const
    {
	uint32 instanceNumber = mayaMeshPath.instanceNumber();
	// get the connected shaders (= materials)
	MObjectArray shaderSets;
	MIntArray shaderVertexIndices;

        MFnMesh mayaMesh(mayaMeshPath, &status);
	checkStatus("couldn't access mesh");

	std::vector<Material> result;
	
	mayaMesh.getConnectedShaders(instanceNumber,
				     shaderSets,
				     shaderVertexIndices);

	for (uint32 i = 0; i < shaderSets.length(); ++i)
	{
	    std::vector<Material> currentMaterials =
		extractMaterials(shaderSets[i]);
	    result.insert(result.end(),
			  currentMaterials.begin(),
			  currentMaterials.end());
	}

	return result;
    }    
} // namespace GPTranslation
