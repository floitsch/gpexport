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
