#pragma once
#include "Base/Platform.hpp"


#if defined(COMPILER_MSVC)
//#define __forceinline
//#define strcpy_s
//#define strcat_s
//#define sprintf_s
//#define memcpy_s
#else
#define __forceinline               inline __attribute__((always_inline))
#define strcpy_s(x, y, z)           strcpy(x, z)
#define strcat_s(x, y, z)           strcat(x, z)
#define sprintf_s                   snprintf
#define memcpy_s(a, b, c, d)        memcpy(a, c, d) 
#endif

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

typedef u32 Bool;     // Defines: True and False
#define True                                1
#define False                               0

typedef void* Handle;
typedef u32 Error;

typedef u32 Id;      // Local id, GUIDs are too slow and unnecessary for our purposes.

typedef char* pstr;
typedef const char* pcstr;

typedef void (*Callback)( void* pUserData );

#define _aligned_malloc(x, y) malloc(x)
#define _aligned_free(x) free(x)


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT( c )
#define ASSERTMSG( c, m )                    
#define ASSERTMSG1( c, m, p1 )
#define ASSERTMSG2( c, m, p1, p2 )
#define ASSERTMSG3( c, m, p1, p2, p3 )
#define ASSERTMSG4( c, m, p1, p2, p3, p4 )

#define COMPILEMSGSTR2( s )                 #s
#define COMPILEMSGSTR( s )                  COMPILEMSGSTR2( s )
#define COMPILEMSGLOC                       __FILE__ "(" COMPILEMSGSTR( __LINE__ ) ") : message: "
#define COMPILE_ASSERT( e )                typedef char __FILE____LINE__[ (e) ? 1 : -1 ]

#define UNREFERENCED_PARAM(P)               
#define DBG_UNREFERENCED_PARAM(P)           
#define DBG_UNREFERENCED_LOCAL_VAR(V)       

#if defined(COMPILER_MSVC)
    #if defined(PLATFORM_X86_64)
    #define W64SAFE
    #else
    #define W64SAFE __w64
    #endif
#else
    #define W64SAFE
#endif

#if defined(PLATFORM_X86_64)
typedef u64 uptr;
#else
typedef W64SAFE u32 uptr;
#endif
