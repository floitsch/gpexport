/*
 * 2004/06/24
 * Florian Loitsch
 *
 * Types used as container for materials
 */

#ifndef ___HPP_GPEXPORT_MATERIAL
#define ___HPP_GPEXPORT_MATERIAL

#include <algorithm>
#include <set>
#include <string>
#include <maya/MColor.h>

#include "basic_types.hpp"

//#define _USE_MATERIAL_CHANNEL_

namespace GPTranslation
{
	#define COLOR_DIFFERENCE_THRESHOLD 0.1f

	typedef struct Channel
	{
		MColor value;
		std::set<std::string> texture;
		
		bool bUseTex()const
		{return !texture.empty();}

		bool operator == ( const Channel& other ) const
		{
			//1. compare texture first.
			if( this->bUseTex() && other.bUseTex())
			{
				if(texture.size() != other.texture.size())
					return false;

				std::set<std::string>::const_iterator i0 = texture.begin();
				std::set<std::string>::const_iterator e0 = texture.end();
				std::set<std::string>::const_iterator i1 = other.texture.begin();
				std::set<std::string>::const_iterator e1 = other.texture.end();

				for(; i0!=e0 && i1!=e1; ++i0, ++i1)
				{
					if( (*i0) != (*i1) )
						return false;
				}
				return  true;
			}
			//2.then compare value.
			else if( !this->bUseTex() && !other.bUseTex())
			{
				bool ret =     (abs(value.r - other.value.r) < COLOR_DIFFERENCE_THRESHOLD)
							&& (abs(value.g - other.value.g) < COLOR_DIFFERENCE_THRESHOLD)
							&& (abs(value.b - other.value.b) < COLOR_DIFFERENCE_THRESHOLD)
							&& (abs(value.a - other.value.a) < COLOR_DIFFERENCE_THRESHOLD);
					return ret;
			}else{
				return false;
			}
		}
	}Channel;

    struct Material
    {
		std::string shadingMode;
		std::string name;

		Channel ambient;
		Channel diffuse;
		//Channel selfIllumination;
		Channel bump;
		Channel color;
		Channel transparency;//opacity
		Channel specularColor;//specular color
		Channel shininess;//specular factor
		Channel incandescence;//self_illum?
		Channel reflection;//?

		bool operator == ( const Material& other ) const
		{
			//If they have the same name, they are the same material in Maya.
			if(name == other.name)
				return true;//
			if(shadingMode != other.shadingMode)
				return false;

			bool ret = (ambient == other.ambient)
				&& ( diffuse== other.diffuse)
				&& ( bump== other.bump)
				&& ( color== other.color)
				&& ( transparency== other.transparency)
				&& ( specularColor== other.specularColor)
				&& ( shininess== other.shininess)
				&& ( incandescence== other.incandescence)
				&& ( reflection== other.reflection);
			return ret;
		}


	};
} // namespace GPTranslation
#endif // include guard
