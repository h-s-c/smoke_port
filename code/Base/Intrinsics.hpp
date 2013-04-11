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
#include "Base/Platform.hpp"
#include <stdint.h>
#include <stdexcept>
#if defined(COMPILER_MSVC)
#   include <intrin.h>
#endif

namespace Base
{
#if defined(COMPILER_GCC) || defined(COMPILER_CLANG) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_GCC))

    uint32_t FORCEINLINE PopCount( uint32_t x )
    {
        return __builtin_popcount(x);
    }
    uint32_t FORCEINLINE BitScanReverse( uint32_t x )
    {
        return __builtin_clz(x);
    }
    uint32_t FORCEINLINE BitScanForward( uint32_t x )
    {
        return __builtin_ctz(x);
    }
    

#elif defined(COMPILER_MSVC) || (defined(COMPILER_ICC) && defined(COMPILER_HOST_MSVC))
    
    uint32_t FORCEINLINE PopCount( uint32_t x )
    {
        uint32_t r = 0;
        r = __popcnt(x);
        return r;
    }
    uint32_t FORCEINLINE BitScanReverse( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanReverse(&r, x);
        return r;
    }
    uint32_t FORCEINLINE BitScanForward( uint32_t x )
    {
        unsigned long r = 0;
        ::_BitScanForward(&r, x);
        return r;
    }

#else
    
    uint32_t FORCEINLINE PopCount( uint32_t x )
    {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    uint32_t FORCEINLINE BitScanReverse( uint32_t x )
    {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - PopCount(x);
    }
    uint32_t FORCEINLINE BitScanForward(( uint32_t x )
    {
        return PopCount((x & -x) - 1);
    }
#endif

}
