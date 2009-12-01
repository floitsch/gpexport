//
//
//
//                MilkShape 3D 1.4.0 and up File Format Specification
//
//
//                  This specifcation is written in C style.
//
//
// The data structures are defined in the order as they appear in the .ms3d file.
//
//
//
//
//

#ifndef __MS3D_H__
#define __MS3D_H__

namespace MS3DTranslation {

// byte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

//
// max values
//
#define MAX_VERTICES    8192
#define MAX_TRIANGLES   16384
#define MAX_GROUPS      128
#define MAX_MATERIALS   128
#define MAX_JOINTS      128
#define MAX_KEYFRAMES   216     // increase when needed



//
// flags
//
#define SELECTED        1
#define HIDDEN          2
#define SELECTED2       4
#define DIRTY           8



//
// types
//
#ifndef byte
typedef unsigned char byte;
#endif // byte

#ifndef word
typedef unsigned short word;
#endif // word



//
// First comes the header.
//
typedef struct
{
    char    id[10];                                     // always "MS3D000000"
    int     version;                                    // 4
} PACK_STRUCT ms3d_header_t;

//
// Then comes the number of vertices
//
//word nNumVertices;

//
// Then comes nNumVertices * sizeof (ms3d_vertex_t)
//
typedef struct
{
    byte    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    float   vertex[3];                                  //
    char    boneId;                                     // -1 = no bone
    byte    referenceCount;
} PACK_STRUCT ms3d_vertex_t;

//
// number of triangles
//
//word nNumTriangles;

//
// nNumTriangles * sizeof (ms3d_triangle_t)
//
typedef struct
{
    word    flags;                                      // SELECTED | SELECTED2 | HIDDEN
    word    vertexIndices[3];                           //
    float   vertexNormals[3][3];                        //
    float   s[3];                                       //
    float   t[3];                                       //
    byte    smoothingGroup;                             // 1 - 32
    byte    groupIndex;                                 //
} PACK_STRUCT ms3d_triangle_t;

//
// number of groups
//
//word nNumGroups;

//
// nNumGroups * sizeof (ms3d_group_t)
//
//
//typedef struct
//{
//    byte            flags;                              // SELECTED | HIDDEN
//    char            name[32];                           //
//    word            numtriangles;                       //
//    word            triangleIndices[numtriangles];      // the groups group the triangles
//    char            materialIndex;                      // -1 = no material
//} PACK_STRUCT ms3d_group_t;

//
// number of materials
//
//word nNumMaterials;

//
// nNumMaterials * sizeof (ms3d_material_t)
//
typedef struct
{
    char            name[32];                           //
    float           ambient[4];                         //
    float           diffuse[4];                         //
    float           specular[4];                        //
    float           emissive[4];                        //
    float           shininess;                          // 0.0f - 128.0f
    float           transparency;                       // 0.0f - 1.0f
    char            mode;                               // 0, 1, 2 is unused now
    char            texture[128];                        // texture.bmp
    char            alphamap[128];                       // alpha.bmp
} PACK_STRUCT ms3d_material_t;

//
// save some keyframer data
//
//float fAnimationFPS;
//float fCurrentTime;
//int iTotalFrames;

//
// number of joints
//
//word nNumJoints;

//
// nNumJoints * sizeof (ms3d_joint_t)
//
//
typedef struct
{
    float           time;                               // time in seconds
    float           rotation[3];                        // x, y, z angles
} PACK_STRUCT ms3d_keyframe_rot_t;

typedef struct
{
    float           time;                               // time in seconds
    float           position[3];                        // local position
} ms3d_keyframe_pos_t PACK_STRUCT;

/*
typedef struct
{
    byte            flags;                              // SELECTED | DIRTY
    char            name[32];                           //
    char            parentName[32];                     //
    float           rotation[3];                        // local reference matrix
    float           position[3];

    word            numKeyFramesRot;                    //
    word            numKeyFramesTrans;                  //

    ms3d_keyframe_rot_t keyFramesRot[numKeyFramesRot];      // local animation matrices
    ms3d_keyframe_pos_t keyFramesTrans[numKeyFramesTrans];  // local animation matrices
} PACK_STRUCT ms3d_joint_t;
*/


//
// Mesh Transformation:
// 
// 0. Build the transformation matrices from the rotation and position
// 1. Multiply the vertices by the inverse of local reference matrix (lmatrix0)
// 2. then translate the result by (lmatrix0 * keyFramesTrans)
// 3. then multiply the result by (lmatrix0 * keyFramesRot)
//
// For normals skip step 2.
//
//
//
// NOTE:  this file format may change in future versions!
//
//
// - Mete Ciragan
//

// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

} // namespace MS3DTranslation

#endif // include guard
