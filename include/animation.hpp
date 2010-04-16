/*
 * 2004/06/24
 * Florian Loitsch
 *
 * data-structures for key-frame skeleton-animations.
 */

#ifndef ___HPP_GPEXPORT_ANIMATION
#define ___HPP_GPEXPORT_ANIMATION

#include <maya/MTime.h>
#include <maya/MMatrix.h>

#include <vector>
#include <string>
#include <map>

#include "basic_types.hpp"

namespace GPTranslation
{
    struct KeyFrame
    {
        MTime   time;
        MMatrix transformation;
    };

    typedef std::vector<KeyFrame> Track;
    typedef std::map<std::string, Track> StringTrackMap;
    
    struct Animation
    {
        uint32              fps;
	MTime               start; // first keyframe. redundant information
				   // (could be extracted out of the tracks)
	
	MTime               end;   // last keyframe. redundant information
				   // (could be extracted out of the tracks)
	
        StringTrackMap      tracks;
    };
} // namespace GPTranslation
#endif // include guard
