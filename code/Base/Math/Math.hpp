// ======================================================================== //
// Copyright 2009-2012 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "Base/Platform.hpp"

#include <cmath>
#include <cfloat>
#include <emmintrin.h>
#include <xmmintrin.h>

namespace Base
{
#if defined(PLATFORM_OS_WINDOWS)
    FORCEINLINE bool Finite ( const float x ) { return _finite(x) != 0; }
#endif

    FORCEINLINE float Sign ( const float x ) { return x<0?-1.0f:1.0f; }
    FORCEINLINE float Sqr  ( const float x ) { return x*x; }
    
#if defined(PLATFORM_ARCH_X86) || defined(PLATFORM_ARCH_X86_64) 
    FORCEINLINE float Rcp  ( const float x ) 
    { 
        const __m128 vx = _mm_set_ss(x);
        const __m128 r = _mm_rcp_ps(vx);
        return _mm_cvtss_f32(_mm_sub_ps(_mm_add_ps(r, r), _mm_mul_ps(_mm_mul_ps(r, r), vx)));
    }
    FORCEINLINE float Signmsk ( const float x ) 
    { 
        return _mm_cvtss_f32(_mm_and_ps(_mm_set_ss(x),_mm_castsi128_ps(_mm_set1_epi32(0x80000000))));
    }
    FORCEINLINE float Xorf( const float x, const float y ) 
    { 
        return _mm_cvtss_f32(_mm_xor_ps(_mm_set_ss(x),_mm_set_ss(y)));
    }
    FORCEINLINE float Rsqrt( const float x ) 
    { 
        const __m128 a = _mm_set_ss(x);
        const __m128 r = _mm_rsqrt_ps(a);
        const __m128 c = _mm_add_ps(_mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
                                    _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r), _mm_mul_ps(r, r)));
        return _mm_cvtss_f32(c);
    }
#endif

    FORCEINLINE float Abs  ( const float x ) { return std::fabs(x); }
    FORCEINLINE float Acos ( const float x ) { return std::acos (x); }
    FORCEINLINE float Asin ( const float x ) { return std::asin (x); }
    FORCEINLINE float Atan ( const float x ) { return std::atan (x); }
    FORCEINLINE float Atan2( const float y, const float x ) { return std::atan2(y, x); }
    FORCEINLINE float Cos  ( const float x ) { return std::cos  (x); }
    FORCEINLINE float Cosh ( const float x ) { return std::cosh (x); }
    FORCEINLINE float Exp  ( const float x ) { return std::expf  (x); }
    FORCEINLINE float Fmod ( const float x, const float y ) { return std::fmod (x, y); }
    FORCEINLINE float Log  ( const float x ) { return std::log  (x); }
    FORCEINLINE float Log10( const float x ) { return std::log10(x); }
    FORCEINLINE float Pow  ( const float x, const float y ) { return std::pow  (x, y); }
    FORCEINLINE float Sin  ( const float x ) { return std::sin  (x); }
    FORCEINLINE float Sinh ( const float x ) { return std::sinh (x); }
    FORCEINLINE float Sqrt ( const float x ) { return std::sqrt (x); }
    FORCEINLINE float Tan  ( const float x ) { return std::tan  (x); }
    FORCEINLINE float Tanh ( const float x ) { return std::tanh (x); }
    FORCEINLINE float Floor( const float x ) { return std::floor (x); }
    FORCEINLINE float Ceil ( const float x ) { return std::ceil (x); }

    FORCEINLINE double Abs  ( const double x ) { return std::fabs(x); }
    FORCEINLINE double Sign ( const double x ) { return x<0?-1.0:1.0; }
    FORCEINLINE double Acos ( const double x ) { return std::acos (x); }
    FORCEINLINE double Asin ( const double x ) { return std::asin (x); }
    FORCEINLINE double Atan ( const double x ) { return std::atan (x); }
    FORCEINLINE double Atan2( const double y, const double x ) { return std::atan2(y, x); }
    FORCEINLINE double Cos  ( const double x ) { return std::cos  (x); }
    FORCEINLINE double Cosh ( const double x ) { return std::cosh (x); }
    FORCEINLINE double Exp  ( const double x ) { return std::exp  (x); }
    FORCEINLINE double Fmod ( const double x, const double y ) { return std::fmod (x, y); }
    FORCEINLINE double Log  ( const double x ) { return std::log  (x); }
    FORCEINLINE double Log10( const double x ) { return std::log10(x); }
    FORCEINLINE double Pow  ( const double x, const double y ) { return std::pow  (x, y); }
    FORCEINLINE double Rcp  ( const double x ) { return 1.0/x; }
    FORCEINLINE double Rsqrt( const double x ) { return 1.0/std::sqrt(x); }
    FORCEINLINE double Sin  ( const double x ) { return std::sin  (x); }
    FORCEINLINE double Sinh ( const double x ) { return std::sinh (x); }
    FORCEINLINE double Sqr  ( const double x ) { return x*x; }
    FORCEINLINE double Sqrt ( const double x ) { return std::sqrt (x); }
    FORCEINLINE double Tan  ( const double x ) { return std::tan  (x); }
    FORCEINLINE double Tanh ( const double x ) { return std::tanh (x); }
    FORCEINLINE double Floor( const double x ) { return std::floor (x); }
    FORCEINLINE double Ceil ( const double x ) { return std::ceil (x); }

    template<typename T> FORCEINLINE T Min(const T& a, const T& b)                                     { return a<b? a:b; }
    template<typename T> FORCEINLINE T Min(const T& a, const T& b, const T& c)                         { return Min(Min(a,b),c); }
    template<typename T> FORCEINLINE T Min(const T& a, const T& b, const T& c, const T& d)             { return Min(Min(a,b),Min(c,d)); }
    template<typename T> FORCEINLINE T Min(const T& a, const T& b, const T& c, const T& d, const T& e) { return Min(Min(Min(a,b),Min(c,d)),e); }

    template<typename T> FORCEINLINE T Max(const T& a, const T& b)                                     { return a<b? b:a; }
    template<typename T> FORCEINLINE T Max(const T& a, const T& b, const T& c)                         { return Max(Max(a,b),c); }
    template<typename T> FORCEINLINE T Max(const T& a, const T& b, const T& c, const T& d)             { return Max(Max(a,b),Max(c,d)); }
    template<typename T> FORCEINLINE T Max(const T& a, const T& b, const T& c, const T& d, const T& e) { return Max(Max(Max(a,b),Max(c,d)),e); }
    
    template<typename T> FORCEINLINE T Clamp(const T& x, const T& lower = T(zero), const T& upper = T(one)) { return Max(lower, Min(x,upper)); }

    template<typename T> FORCEINLINE T  Deg2Rad ( const T& x )  { return x * T(1.74532925199432957692e-2f); }
    template<typename T> FORCEINLINE T  Rad2Deg ( const T& x )  { return x * T(5.72957795130823208768e1f); }
    template<typename T> FORCEINLINE T  Sin2Cos ( const T& x )  { return Sqrt(max(T(zero),T(one)-x*x)); }
    template<typename T> FORCEINLINE T  Cos2Sin ( const T& x )  { return Sin2Cos(x); }

    /*! selects */
    FORCEINLINE bool  Select(bool s, bool  t , bool f) { return s ? t : f; }
    FORCEINLINE int   Select(bool s, int   t,   int f) { return s ? t : f; }
    FORCEINLINE float Select(bool s, float t, float f) { return s ? t : f; }
}
