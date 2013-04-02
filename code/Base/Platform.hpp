// public doamin

#pragma once

/* detect 32 or 64 platform */
#if defined(__x86_64__) || defined(__ia64__) || defined(_M_X64)
#define PLATFORM_X86_64
#else
#define PLATFORM_X86
#endif

/* detect Linux platform */
#if defined(linux) || defined(__linux__) || defined(__LINUX__)
#  if !defined(PLATFORM_LINUX)
#     define PLATFORM_LINUX
#  endif
#  if !defined(PLATFORM_UNIX)
#     define PLATFORM_UNIX
#  endif
#endif

/* detect FreeBSD platform */
#if defined(__FreeBSD__) || defined(__FREEBSD__)
#  if !defined(PLATFORM_FREEBSD)
#     define PLATFORM_FREEBSD
#  endif
#  if !defined(PLATFORM_UNIX)
#     define PLATFORM_UNIX
#  endif
#endif

/* detect Windows 95/98/NT/2000/XP/Vista/7 platform */
#if (defined(WIN32) || defined(WIN64) ||defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
#  if !defined(PLATFORM_WINDOWS)
#     define PLATFORM_WINDOWS
#  endif
#endif

/* detect Cygwin platform */
#if defined(__CYGWIN__)
#  if !defined(PLATFORM_UNIX)
#     define PLATFORM_UNIX
#  endif
#endif

/* detect MAC OS X platform */
#if defined(__APPLE__) || defined(MACOSX) || defined(__MACOSX__)
#  if !defined(PLATFORM_MACOSX)
#     define PLATFORM_MACOSX
#  endif
#  if !defined(PLATFORM_UNIX)
#     define PLATFORM_UNIX
#  endif
#endif

/* try to detect other Unix systems */
#if defined(__unix__) || defined (unix) || defined(__unix) || defined(_unix)
#  if !defined(PLATFORM_UNIX)
#     define PLATFORM_UNIX
#  endif
#endif

#if defined(_MSC_VER)
#  if !defined(COMPILER_MSVC)
#     define COMPILER_MSVC
#  endif
#endif

#if defined(__GNUC__)
#  if !defined(COMPILER_GCC)
#     define COMPILER_GCC
#  endif
#endif

#if defined(__clang__)
#  if !defined(COMPILER_CLANG)
#     define COMPILER_CLANG
#  endif
#endif

#if defined(COMPILER_MSVC)
#define NOINLINE            __declspec(noinline)
#define INLINE              __forceinline
#define ALIGN(...)          __declspec(align(__VA_ARGS__))
#define STDCALL             __stdcall
#define DLLEXPORT           __declspec(dllexport)
#else
#define NOINLINE            __attribute__((noinline))
#define INLINE              inline __attribute__((always_inline))
#define ALIGN(...)          __attribute__((aligned(__VA_ARGS__)))
#define STDCALL   
#define DLLEXPORT            
#endif

#define SAFE_DELETE( p )                    if ((p)!=nullptr){delete (p); (p)=nullptr;}
#define SAFE_DELETE_ARRAY( p )              if ((p)!=nullptr){delete [] (p); (p)=nullptr;}

#if defined(COMPILER_MSVC)
#define In                                  __in const
#define Out                                 __out
#define InOut                               __inout
#else
#define In                                  const
#define Out
#define InOut
#endif

#if defined(PLATFORM_UNIX)
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#endif

