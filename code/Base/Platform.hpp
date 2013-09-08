/* The MIT License (MIT)
 * Copyright (c) 2013 Kevin Schmidt
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
 
#pragma once

/* detect 32 or 64 platform */
#if defined(__x86_64__) || defined(__ia64__) || defined(_M_X64)
#   define PLATFORM_ARCH_X86_64
#else
#   define PLATFORM_ARCH_X86
#endif

/* try to detect other Unix systems */
#if defined(__unix__) || defined (unix) || defined(__unix) || defined(_unix)
#   if !defined(PLATFORM_OS_UNIX)
#       define PLATFORM_OS_UNIX
#   endif
#endif

/* detect Linux platform */
#if defined(linux) || defined(__linux__) || defined(__LINUX__)
#   define PLATFORM_OS_LINUX
#   define PLATFORM_OS_UNIX
#endif

/* detect FreeBSD platform */
#if defined(__FreeBSD__) || defined(__FREEBSD__)
#   define PLATFORM_OS_FREEBSD
#   define PLATFORM_OS_UNIX
#endif


/* detect Windows CE platform */
#if defined(_WIN32_WCE) || defined(WIN32_PLATFORM_HPCPRO) || defined(WIN32_PLATFORM_HPC2000)
/* _WIN32_WCE == 300 for Pocket PC
 * _WIN32_WCE == 211 for Palm-size PC 2.11 (Wyvern)
 * _WIN32_WCE == 201 for Palm-size PC 2.01 (Gryphon) 
 * WIN32_PLATFORM_HPC2000 for H/PC 2000 (Galileo)*/
#endif

/* detect Windows 95/98/NT/2000/XP/Vista/7/8 platform */
#if (defined(WIN32) || defined(WIN64) ||defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
#define PLATFORM_OS_WINDOWS
#endif

/* detect Cygwin platform */
#if defined(__CYGWIN__)
#   define PLATFORM_OS_CYGWIN
#endif

/* detect MAC OS X platform */
#if defined(__APPLE__) || defined(MACOSX) || defined(__MACOSX__)
#   define PLATFORM_OS_MACOSX
#   if !defined(PLATFORM_OS_UNIX)
#       define PLATFORM_OS_UNIX
#   endif
#endif

/* detect Clang */
#if defined(__clang__) || defined(__llvm__)
#   define COMPILER_CLANG
#   if defined(__GNUC__)
#       define COMPILER_HOST_GCC
#   endif
#   define COMPILER_VERSION_MAJOR __clang_major__
#   define COMPILER_VERSION_MINOR __clang_minor__
#   define COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

/* detect Intel C/C++ Compiler */
#if defined(__ICC) || defined(__INTEL_COMPILER)
#   define COMPILER_ICC
#   if defined(__GNUC__)
#       define COMPILER_HOST_GCC
#   endif
#   if defined(_MSC_VER)
#       define COMPILER_HOST_MSVC
#   endif
#   if __INTEL_COMPILER_BUILD_DATE == 20130118
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 2
#   endif
#   if __INTEL_COMPILER_BUILD_DATE == 20120731
#       define COMPILER_VERSION_MAJOR 13
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if __INTEL_COMPILER_BUILD_DATE == 20120928
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 7
#   endif
#   if __INTEL_COMPILER_BUILD_DATE == 20120130
#       define COMPILER_VERSION_MAJOR 12
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 3
#   endif
#   if __INTEL_COMPILER_BUILD_DATE == 20060606
#       define COMPILER_VERSION_MAJOR 8
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 038
#   endif
#endif

/* detect Microsoft Visual C/C++ */
#if defined(_MSC_VER) && !defined(COMPILER_ICC)
#   define COMPILER_MSVC
#   if _MSC_VER == 1700 /*Visual Studio 2012*/
#       define COMPILER_VERSION_MAJOR 11
#       if _MSC_FULL_VER == 170051106 /*Update 1 RTM*/
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 1
#       endif
#       if _MSC_FULL_VER == 170051025 /*Update 1 CTP 1*/
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#       if _MSC_FULL_VER == 170050727 /*RTM*/
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#       if _MSC_FULL_VER == 170050214 /*Beta 1*/
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#       if _MSC_FULL_VER == 170040825 /*CTP*/
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#   endif
// _MSC_FULL_VER: 160011001 (CTP), 160020506 (beta1), 160021003 (beta2), 160030128 (rc), 160040219 (sp1)
#   if _MSC_VER == 1600 /*Visual Studio 2010*/
#       define COMPILER_VERSION_MAJOR 10
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1500 /*Visual Studio 2008*/
#       define COMPILER_VERSION_MAJOR 9
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1400 /*Visual Studio 2005*/
#       define COMPILER_VERSION_MAJOR 8
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1310 /*Visual Studio.NET 2003*/
#       define COMPILER_VERSION_MAJOR 7
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1300 /*Visual Studio.NET 2002*/
#       define COMPILER_VERSION_MAJOR 7
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1200 /*Visual Studio 6.0*/
#       define COMPILER_VERSION_MAJOR 6
#       if _MSC_FULL_VER == 12008804 /*SP6*/
#           define COMPILER_VERSION_MINOR 6
#           define COMPILER_VERSION_PATCH 0
#       elseif
#           define COMPILER_VERSION_MINOR 0
#           define COMPILER_VERSION_PATCH 0
#       endif
#   endif
#   if _MSC_VER == 1100 /*Visual Studio 97*/
#       define COMPILER_VERSION_MAJOR 5
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1020 /*Visual C++ 4.2*/
#       define COMPILER_VERSION_MAJOR 4
#       define COMPILER_VERSION_MINOR 2
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1010 /*Visual C++ 4.1*/
#       define COMPILER_VERSION_MAJOR 4
#       define COMPILER_VERSION_MINOR 1
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 1000 /*Visual C++ 4.0*/
#       define COMPILER_VERSION_MAJOR 4
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 900 /*Visual C++ 2.0 */
#       define COMPILER_VERSION_MAJOR 2
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 800 /*Visual C++ 1.0*/
#       define COMPILER_VERSION_MAJOR 1
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 700 /*C/C++ 7.0*/
#       define COMPILER_VERSION_MAJOR 0
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 600 /*C Compiler 6.x*/
#       define COMPILER_VERSION_MAJOR 0
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#   if _MSC_VER == 510 /*C Compiler 5.1*/
#       define COMPILER_VERSION_MAJOR 0
#       define COMPILER_VERSION_MINOR 0
#       define COMPILER_VERSION_PATCH 0
#   endif
#endif

/* detect GNU Compiler Collection */
#if defined(__GNUC__) && !defined(COMPILER_ICC) && !defined(COMPILER_CLANG)
#   define COMPILER_GCC
#   define COMPILER_VERSION_MAJOR __GNUC__
#   define COMPILER_VERSION_MINOR __GNUC_MINOR__
#   define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#endif

/* detect Dinkumware standard library */
#if defined(_CPPLIB_VER)
#   define COMPILER_STDLIB_DINKUMWARE
#   define COMPILER_STDLIB_VERSION _CPPLIB_VER
#endif

/* detect STLPort standard library */
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#   define COMPILER_STDLIB_STLPORT
#   define COMPILER_STDLIB_VERSION _STLPORT_VERSION
#endif

/* detect GNU libstdc++ standard library */
#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#   define COMPILER_STDLIB_GLIBC
#   define COMPILER_STDLIB_VERSION __GLIBC__##__GLIBC__MINOR__
#endif

/* detect uClibc standard library */
#if defined(__UCLIBC__)
#   define COMPILER_STDLIB_UCLIBC
#   define COMPILER_STDLIB_VERSION __UCLIBC_MAJOR__##__UCLIBC_MINOR__##__UCLIBC_SUBLEVEL__
#endif

/* detect Bionic libc standard library */
#if defined(__BIONIC__)
#   define COMPILER_STDLIB_BIONIC
#   define COMPILER_STDLIB_VERSION
#endif

/* cross-platform macros */
#if defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
#   define COMPILER_NOINLINE             __declspec(noinline)
#   define COMPILER_INLINE               __inline
#   define COMPILER_FORCEINLINE          __forceinline
#   define COMPILER_ALIGN(...)           __declspec(align(__VA_ARGS__))
#   define COMPILER_DLLEXPORT            __declspec(dllexport)
#   define COMPILER_USED
#else
#   define COMPILER_NOINLINE             __attribute__((noinline))
#   define COMPILER_INLINE               inline
#   define COMPILER_FORCEINLINE          inline __attribute__((always_inline))
#   define COMPILER_ALIGN(...)           __attribute__((aligned(__VA_ARGS__)))
#   define COMPILER_DLLEXPORT      
#   define COMPILER_COMPILER_USED        __attribute__((used))      
#endif

/* C++11 workarounds */
#if !defined(thread_local)
#   if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC))
#       define __thread_local __thread
#   elif defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
#       define __thread_local __declspec(thread)
#   endif
#else
#   define __thread_local thread_local
#endif
