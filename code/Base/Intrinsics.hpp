// public domain

#pragma once

#include "Base/Platform.hpp"

#include <stdint.h>
#include <stdexcept>

#if defined(COMPILER_MSVC)
#include <intrin.h>
#endif

namespace Base
{
    #if defined (COMPILER_GCC) || defined  (COMPILER_CLANG)

    uint32_t INLINE PopCount( uint32_t x )
    {
        return __builtin_popcount(x);
    }
    uint32_t INLINE BitScanReverse( uint32_t x )
    {
        return __builtin_clz(x);
    }
    uint32_t INLINE BitScanForward( uint32_t x )
    {
        return __builtin_ctz(x);
    }
    

    #elif defined(COMPILER_MSVC)
    
    uint32_t INLINE PopCount( uint32_t x )
    {
        uint32_t r = 0;
        r = __popcnt(x);
        return r;
    }
    uint32_t INLINE BitScanReverse( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanReverse(&r, x);
        return r;
    }
    uint32_t INLINE BitScanForward( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanForward(&r, x);
        return r;
    }

    #else
    
    uint32_t INLINE PopCount( uint32_t x )
    {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    uint32_t INLINE BitScanReverse( uint32_t x )
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - PopCount(x);
    }
    uint32_t INLINE BitScanForward(( uint32_t x )
    {
        return PopCount((x & -x) - 1);
    }
    #endif

}
