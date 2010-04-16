/*
 * 2004/06/24
 * Florian Loitsch
 *
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
