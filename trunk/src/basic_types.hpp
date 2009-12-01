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
 * basic Type(def)s
 */

#ifndef ___HPP_GPEXPORT_BASIC_TYPES
#define ___HPP_GPEXPORT_BASIC_TYPES

namespace GPTranslation
{
    typedef unsigned int    uint32;
    typedef int             int32;
    typedef unsigned char   byte;

    struct Float2
    {
        union
        {
            float f[2];
            struct { float x; float y; };
            struct { float u; float v; };
            struct { float s; float t; };
        };
    };

    struct Int3
    {
        union
        {
            int i[3];
            struct { int i1; int i2; int i3; };
        };
    };

} // namespace GPTranslation
#endif // include guard
