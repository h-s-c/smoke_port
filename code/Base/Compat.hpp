// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#pragma once
#include "Base/Platform.hpp"


#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
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

#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
    #if defined(PLATFORM_ARCH_X86_64)
    #define W64SAFE
    #else
    #define W64SAFE __w64
    #endif
#else
    #define W64SAFE
#endif

#if defined(PLATFORM_ARCH_X86_64)
typedef u64 uptr;
#else
typedef W64SAFE u32 uptr;
#endif

#define SAFE_DELETE( p )                    if ((p)!=nullptr){delete (p); (p)=nullptr;}
#define SAFE_DELETE_ARRAY( p )              if ((p)!=nullptr){delete [] (p); (p)=nullptr;}

#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
#define In                                  __in const
#define Out                                 __out
#define InOut                               __inout
#else
#define In                                  const
#define Out
#define InOut
#endif

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC))
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#endif
