#pragma once

#include <stdint.h>
#include <stdexcept>

#if defined(COMPILER_MSVC)
#include <intrin.h>
#endif

namespace Base
{
    #if defined (COMPILER_GCC)
    
    uint64_t INLINE TimeStampCounter() 
    {
        #if defined (__X86__)
        unsigned long long a;
        asm volatile("rdtsc":"=A" (a));
        return a;
        #elif defined __x86_64
        unsigned int _hi,_lo;
        asm volatile("rdtsc":"=a"(_lo),"=d"(_hi));
        return ((unsigned long long int)_hi << 32) | _lo;
        #endif
    }

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
    
    long INLINE InterlockedCompareExchange( long * value, long  comp_val, long  new_val )
    {
        __sync_synchronize();
        return __sync_val_compare_and_swap( value, comp_val, new_val );
    }
    long INLINE InterlockedExchange(long * Target, long Value)
    {
        __sync_synchronize();
        return __sync_lock_test_and_set(Target, Value);
    }

    #elif defined(COMPILER_MSVC)
    
    uint64_t INLINE TimeStampCounter() 
    {
        return __rdtsc();
    }
    uint32_t INLINE PopCount( uint32_t x )
    {
        uint32_t r = 0;
        r = __popcnt(x)
        return r;
    }
    uint32_t INLINE BitScanReverse( uint32_t x )
    {
        int32_t r = 0;
        _BitScanReverse(&r, x);
        return r;
    }
    uint32_t INLINE BitScanForward(( uint32_t x )
    {
        int32_t r = 0;
        _BitScanForward(&r, x);
        return r;
    }
    long INLINE InterlockedCompareExchange( long * value, long  comp_val, long  new_val )
    {
        return _InterlockedCompareExchange( value, comp_val, new_val );
    }
    long INLINE InterlockedExchange(long * Target, long Value)
    {
        return _InterlockedExchange(Target, Value);
    }

    #else

    uint64_t INLINE TimeStampCounter() 
    {
        //fall back to get time of day
        timeval t;
        gettimeofday(&t, NULL);
        return ((sint64)t.tv_sec) * 1000 + t.tv_usec;
    }

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
    
    long INLINE InterlockedCompareExchange( long * value, long  comp_val, long  new_val )
    {
        throw std::runtime_error("Not implmented");
    }    
    
    long INLINE InterlockedExchange(long * Target, long Value)
    {
        throw std::runtime_error("Not implmented");
    }    

    #endif

}
