/*
 * Extracts all Material-related data of Maya.
 */

#include "materialExtraction.hpp"

#include <iostream>

#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFileObject.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnLambertShader.h>
#include <maya/MPlugArray.h>
#include <maya/MFnMesh.h>
#include "util.h"
#include "gplog.h"

std::ostream& operator<< ( std::ostream &os, const MColor &c )
{  
	os  << c.r << "," << c.g << "," << c.b << "," << c.a ;
	return os;  
} 

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
			_GPDEBUG(" textureFile ="<< textureFile);	

			if(textureFile.length()!=0)//  [10/29/2009 yys] has texture
			{
				// small trick (seen in X-file exporter) to get the relative path:
				MFileObject filePath;

				//filePath.setFullName(textureFile);
				filePath.setRawFullName(textureFile);//  [10/29/2009 yys]

				// Note, that the index in the textures-vector represents the
				// texCoord-index. (uv-index)
				textures.push_back(filePath.resolvedFullName().asChar());//  [10/29/2009 yys]
				_GPDEBUG("resolvedFullName\t\t"	<<filePath.resolvedFullName());
			}

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
	//-----------------------Specular-----------------------------
	template<typename T>
	MColor MaterialExtractor::getSpecular(const T& shader) const
	{ throw "not implemented"; }

	template<>
	MColor MaterialExtractor::getSpecular<MFnPhongShader>
		(const MFnPhongShader& shader) const
	{ return shader.specularColor(); }

	template<>
	MColor MaterialExtractor::getSpecular<MFnBlinnShader>
		(const MFnBlinnShader& shader) const
	{ return shader.specularColor(); }

	template<>
	MColor MaterialExtractor::getSpecular<MFnLambertShader>
		(const MFnLambertShader& shader) const
	{ return MColor(0.0f, 0.0f, 0.0f); }
    /*
     * fills the given material.
     */
	template<typename T>
	void
		MaterialExtractor::fillMaterial2(Material &material,
		const MObject& shaderNode,
		const std::string& shadingType) const
	{
		_GPDEBUG("\n\nMATERIAL: "<< material.name );
		T shader(shaderNode);
		std::string shadingMode;
		if (shadingType == "phong")
			material.shadingMode = "phong";
		else // blinn and lambert
			material.shadingMode = "gouraud";

		_GPDEBUG("\t channel value:");
		_GPDEBUG("\t ambient =" <<		shader.ambientColor());
		_GPDEBUG("\t diffuseCoeff =" <<	shader.diffuseCoeff());
		_GPDEBUG("\t color =" <<			shader.color()	);
		_GPDEBUG("\t incandescence =" <<	shader.incandescence());
		_GPDEBUG("\t shininess =" <<		getShininess(shader));
		_GPDEBUG("\t transparency =" <<	shader.transparency());
		_GPDEBUG("\t specular =" <<		getSpecular(shader));
		
		_GPDEBUG("\n");		
		_GPDEBUG("\t channel texture:");
		MFnDependencyNode  fn( shaderNode );
		OutputColor2(fn,"ambientColor", material.ambient);
		OutputColor2(fn,"diffuse", material.diffuse);
		OutputColor2(fn,"color", material.color);
		OutputColor2(fn,"specularColor", material.specularColor);
		OutputColor2(fn,"incandescence", material.incandescence);
		OutputColor2(fn,"transparency", material.transparency);
		OutputBumpMap(fn, material.bump);
		//_GPDEBUG("\tcos "<<fn.cosPower() ); 

		material.ambient.value = shader.ambientColor();
		if( material.color.bUseTex() ){
			material.color.value = MColor(1.0, 1.0, 1.0, 1.0);
		}
		material.diffuse.value = shader.diffuseCoeff() * MColor(1.0, 1.0, 1.0, 1.0);
		material.incandescence.value = shader.incandescence();
		material.transparency.value = shader.transparency();
		material.specularColor.value = getSpecular(shader);
		material.shininess.value.r 
			= material.shininess.value.g 
			= material.shininess.value.b 
			= getShininess(shader);
	}

    void 
	MaterialExtractor::extractTextureName(const MObject& shaderSet)const
	{
		MStatus status;

		// The material node.
		const MObject &material = shaderSet;

		// The attribute to which may be connected a texture.
		const MString COLOR_ATTR( "outColor" );//"color"

		// The attribute on Maya's "file" node which stores the
		// path to the assigned texture image.
		const MString TEXTURE_NAME( "fileTextureName" );
		
		MFnDependencyNode fnDependNode( material );
		MPlug plug = fnDependNode.findPlug( COLOR_ATTR, &status );
		if ( status != MS::kSuccess ){
			_GPDEBUG("CAN NOT FIND PLUG COLOR.");
			return ;
		}
		
		
		// If you're looping through arbitrary attributes here, you could
		// add a check to ensure that the queried plug represents a
		// (double3) attribute, and perhaps even MFnAttribute::isUsedAsColor().

		// Is plug connected?
		MPlugArray cc;

		// Do not simply check for return value here; this returns TRUE if the
		// the plug is _either_ the source or destination for a connection,
		// even though we've specified only destination.
		plug.connectedTo( cc, true /* asDst */, false );
		if ( cc.length() > 0 )
		{
			// As a destination there can be only one source.
			MObject src = cc[0];

			// Is it a file texture?
			if ( src.hasFn( MFn::kFileTexture ) )
			{
				MFnDependencyNode fnFile( src );
				MPlug ftnPlug = fnFile.findPlug( TEXTURE_NAME, &status );
				if ( status == MS::kSuccess )
				{
					MString fileTextureName;
					ftnPlug.getValue( fileTextureName );
					_GPDEBUG("TEXTURE NAME :" << fileTextureName);
				}
			}
			// Not a file node? Add support for other nodes here.
			else{
				//assert( false && "Unsupported texture node." );
				_GPDEBUG("Unsupported texture node.");
			}
		}else{
			// The plug is not connected. Just read its color values.
			double r, g, b;
			plug.child(0).getValue( r );
			plug.child(1).getValue( g );
			plug.child(2).getValue( b );
			_GPDEBUG("  r=" << r << ", g="<<g <<", b="<< b );
		}
	
	}
	/*
     * for the given shader-set extracts all materials.
     */
    std::vector<Material>
    MaterialExtractor::extractMaterials(const MObject& shaderSet) const
    {
		std::vector<Material> materials;

		extractTextureName(shaderSet);
		
		MFnDependencyNode depNode(shaderSet);
		_GPDEBUG("shaderSet name: " << depNode.name().asChar());
		MPlug shaderPlug = depNode.findPlug("surfaceShader");
		
		if (shaderPlug.isNull()) return materials; // empty vector
		
		MPlugArray shaderPlugSources;
		shaderPlug.connectedTo(shaderPlugSources, true, false);
		_GPDEBUG("shader Plug Sources count: " << shaderPlugSources.length() );
		for (uint32 i = 0; i < shaderPlugSources.length(); ++i)
		{
			Material currentMaterial;

			MPlug currentShaderPlugSource = shaderPlugSources[i];
			currentMaterial.name = getTableNameOfShaderPlugSource(currentShaderPlugSource);

			MObject shaderNode = currentShaderPlugSource.node();
			if (shaderNode.hasFn(MFn::kPhong))
			{
				fillMaterial2<MFnPhongShader>(currentMaterial, shaderNode, "phong");
			}else if (shaderNode.hasFn(MFn::kBlinn))
			{
				fillMaterial2<MFnBlinnShader>(currentMaterial, shaderNode, "blinn");
			}else if (shaderNode.hasFn(MFn::kLambert))
			{
				fillMaterial2<MFnLambertShader>(currentMaterial, shaderNode, "lambert");		
			}else
			{
			// don't know how to extract it
			//continue;//  [10/29/2009 yys] 'continue' will skip the following task.
			}

// 			currentMaterial.texturePaths =
// 			extractTextures(currentShaderPlugSource);

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
		_GPDEBUG("Mesh: " << mayaMesh.name().asChar());

		std::vector<Material> result;
		
		mayaMesh.getConnectedShaders(instanceNumber,
						 shaderSets,
						 shaderVertexIndices);
// 		printMObjectArray("shaderSets: ", shaderSets);
// 		printArray("shaderVertexIndices: ", shaderVertexIndices);

		//
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



	std::string MaterialExtractor::getTableNameOfShaderPlugSource(const MPlug &plugsource) const
	{
		std::string plugSrcName(plugsource.name().asChar());

		size_t found;
		found = plugSrcName.find_last_of('.');
		return plugSrcName.substr(0,found);
	}

	bool MaterialExtractor::OutputColor(MFnDependencyNode& fn,const char* name) const
	{
		MPlug p;

		MString r = name;
		r += "R";
		MString g = name;
		g += "G";
		MString b = name;
		b += "B";
		MString a = name;
		a += "A";

		// get the color value
		MColor color;

		// get a plug to the attribute
		p = fn.findPlug(r.asChar());
		p.getValue(color.r);
		p = fn.findPlug(g.asChar());
		p.getValue(color.g);
		p = fn.findPlug(b.asChar());
		p.getValue(color.b);
		p = fn.findPlug(a.asChar());
		p.getValue(color.a);
		p = fn.findPlug(name);

		// will hold the txture node name
		MString texname;
		MString imgname;

		// get plugs connected to colour attribute
		MPlugArray plugs;
		p.connectedTo(plugs,true,false);

		// see if any file textures are present
		for(int i=0;i!=plugs.length();++i)
		{
			// if file texture found
			if(plugs[i].node().apiType() == MFn::kFileTexture)
			{
				// bind a function set to it ....
				MFnDependencyNode fnDep(plugs[i].node());

				// to get the node name
				texname = fnDep.name();
				fnDep.findPlug("fileTextureName").getValue(imgname);

				// stop looping
				break;

			}

		}
		if( 	name == "color" &&
			color.r <0.01 &&
			color.g < 0.01 &&
			color.b < 0.01)

		{
			color.r=color.g=color.b=0.6f;

		}
		// output the name, color and texture ID
		_GPDEBUG("\t" << name << " "
			<< color.r << " "
			<< color.g << " "
			<< color.b << " "
			<< color.a << " tex= "
			<< texname.asChar() << " "
			<< imgname.asChar() << " ");

		return true;

	} 
	bool MaterialExtractor::OutputColor2(MFnDependencyNode& fn,const char* name, GPTranslation::Channel &channel) const
	{
		MPlug p;
		MStatus status;

		MString r = name;
		r += "R";
		MString g = name;
		g += "G";
		MString b = name;
		b += "B";
		MString a = name;
		a += "A";

		// get the color value
		MColor color;

		// get a plug to the attribute
		p = fn.findPlug(r.asChar(), &status);
		if(status==MS::kSuccess){
			p.getValue(color.r);
		} else {
			_GPDEBUG("\t" << name << ": no r, \t") ;
		}

		p = fn.findPlug(g.asChar(), &status);
		if(status==MS::kSuccess){
			p.getValue(color.g);
		} else {
			_GPDEBUG("\t" << name << ": no g, \t");
		}
		
		p = fn.findPlug(b.asChar(), &status);
		if(status==MS::kSuccess){
			p.getValue(color.b);
		} else {
			_GPDEBUG("\t" << name << ": no b, \t");
		}

		p = fn.findPlug(a.asChar(), &status);
		if(status==MS::kSuccess){
			p.getValue(color.a);
		} else {
			_GPDEBUG("\t" << name << ": no a, \t");
		}

		p = fn.findPlug(name, &status);
		if(status!=MS::kSuccess){
			_GPDEBUG("\tCAN NOT FIND PLUG: "<< name);
		}

		// will hold the txture node name
		MString texname;
		MString imgname;

		// get plugs connected to colour attribute
		MPlugArray plugs;
		p.connectedTo(plugs,true,false);

		// see if any file textures are present
		for(int i=0;i!=plugs.length();++i)
		{
			// if file texture found
			if(plugs[i].node().apiType() == MFn::kFileTexture)
			{
				// bind a function set to it ....
				MFnDependencyNode fnDep(plugs[i].node());

				// to get the node name
				texname = fnDep.name();
				fnDep.findPlug("fileTextureName").getValue(imgname);

				// stop looping
				break;
			}
		}
		if( 	name == "color" &&
			color.r <0.01 &&
			color.g < 0.01 &&
			color.b < 0.01)
		{
			color.r=color.g=color.b=0.6f;
		}
		// output the name, color and texture ID
		_GPDEBUG("\t" << name << " "
			<< color.r << " "
			<< color.g << " "
			<< color.b << " "
			<< color.a << " tex= "
			<< texname.asChar() << ", "
			<< imgname.asChar() << ", ");
		channel.value = color;
		if(imgname.length()!=0){
			channel.texture.insert( imgname.asChar() );
		}

		return true;
	} 

	// create an iterator to go through all meshes
	void MaterialExtractor::OutputBumpMap(MFnDependencyNode& fn, GPTranslation::Channel &bump) const
	{
		// get a plug to the normalCamera attribute on the material
		MPlug bump_plug = fn.findPlug("normalCamera");
		MPlugArray connections;

		// get connections to the attribute
		bump_plug.connectedTo(connections,true,false);

		// loop through each one to find a bump2d node
		for(unsigned int i=0;i<connections.length();++i)
		{
			if (connections[i].node().apiType() == MFn::kBump)
			{
				// attach a function set to the 2d bump node
				MFnDependencyNode fnBump(connections[i].node());
				float bump_depth;
				// get the bump depth value from the node
				MPlug bumpDepth = fnBump.findPlug("bumpDepth");
				bumpDepth.getValue(bump_depth);

				// we now have the fun and joy of actually finding
				// the file node that is connected to the bump map
				// node itself. This is going to involve checking
				// the attribute connections to the bumpValue attribute.
				MPlug bump_value = fnBump.findPlug("bumpValue");
				MPlugArray bv_connections;

				bump_value.connectedTo(bv_connections,true,false);
				for(unsigned int j=0;j<bv_connections.length();++j)
				{
					if(bv_connections[i].node().apiType() == MFn::kFileTexture)
					{
						// we have a texture. determine which texture it is.
						MFnDependencyNode fnTexNode(bv_connections[i].node());
						MString textureFile;
						fnTexNode.findPlug("fileTextureName").getValue(textureFile);

						float coverageU,coverageV;
						fnTexNode.findPlug("coverageU").getValue(coverageU);
						fnTexNode.findPlug("coverageV").getValue(coverageV);

						_GPDEBUG( "\t\t bumpmap: "
							<< "\t\t\t bumpdepth " << bump_depth << endl
							<< "\t\t\t texture_node " << fnTexNode.name() << endl
							<< "\t\t\t texture file" << textureFile  << endl
							<< "\t\t\t coverage:" << coverageU << ", " << coverageV << endl
							);
						//
						bump.texture.insert(textureFile.asChar());
						bump.value = bump_depth;

						return;
					}
				}
			}
		}
		return;

	} 

} // namespace GPTranslation
