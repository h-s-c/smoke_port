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

#include "Base/Math/SSE/SSE.hpp"
#include "Base/Math/SSE/Math.hpp"

/* Usage: see Base/Math/Color3.hpp
 */

namespace Base
{
////////////////////////////////////////////////////////////////////////////////////////////////////
/// SSE RGB Color3<float> Class
////////////////////////////////////////////////////////////////////////////////////////////////////

    template<> struct Color3<float>
    {
        union 
        {
          __m128 m128;
          struct { float r,g,b; int dummy; };
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Color3           ( )                   { }
        COMPILER_FORCEINLINE Color3           ( const __m128 a           ) : m128(a) {}
        COMPILER_FORCEINLINE Color3           ( const Color3<float>& other ) : m128(other.m128) {}
        COMPILER_FORCEINLINE Color3<float>& operator=( const Color3<float>& other ) { m128 = other.m128; return *this; }

        COMPILER_FORCEINLINE explicit Color3 (const float v)                               : m128(_mm_set1_ps(v)) {}
        COMPILER_FORCEINLINE explicit Color3 (const float* v, int stride = 1)              : m128(_mm_set_ps(0.0f,v[2*stride],v[stride],v[0])) {}
        COMPILER_FORCEINLINE          Color3 (const float r, const float g, const float b) : m128(_mm_set_ps(0.0f,b,g,r)) {}

        COMPILER_FORCEINLINE operator const __m128&( void ) const { return m128; }
        COMPILER_FORCEINLINE operator       __m128&( void )       { return m128; }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Color3( ZeroTy   ) : m128(_mm_set1_ps(0.0f)) {}
        COMPILER_FORCEINLINE Color3( OneTy    ) : m128(_mm_set1_ps(1.0f)) {}
        COMPILER_FORCEINLINE Color3( PosInfTy ) : m128(_mm_set1_ps(pos_inf)) {}
        COMPILER_FORCEINLINE Color3( NegInfTy ) : m128(_mm_set1_ps(neg_inf)) {}
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Color3<float> operator +( const Color3<float>& a ) { return a; }
    COMPILER_FORCEINLINE const Color3<float> operator -( const Color3<float>& a ) 
    {
        const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
        return _mm_xor_ps(a.m128, mask);
    }
    COMPILER_FORCEINLINE const Color3<float> Abs  ( const Color3<float>& a ) 
    {
        const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
        return _mm_and_ps(a.m128, mask);
    }
    COMPILER_FORCEINLINE const Color3<float> Rcp  ( const Color3<float>& a ) 
    {
        const Color3<float> r = _mm_Rcp_ps(a.m128);
        return _mm_sub_ps(_mm_add_ps(r, r), _mm_mul_ps(_mm_mul_ps(r, r), a));
    }
    COMPILER_FORCEINLINE const Color3<float> Rsqrt( const Color3<float>& a ) 
    {
        __m128 r = _mm_Rsqrt_ps(a.m128);
        return _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.5f),r), _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, _mm_set1_ps(-0.5f)), r), _mm_mul_ps(r, r)));
    }
    COMPILER_FORCEINLINE const Color3<float> Sqrt ( const Color3<float>& a ) { return _mm_sqrt_ps(a.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Color3<float> operator +( const Color3<float>& a, const Color3<float>& b ) { return _mm_add_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Color3<float> operator -( const Color3<float>& a, const Color3<float>& b ) { return _mm_sub_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Color3<float> operator *( const Color3<float>& a, const Color3<float>& b ) { return _mm_mul_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Color3<float> operator *( const Color3<float>& a, const float b        ) { return a * Color3<float>(b); }
    COMPILER_FORCEINLINE const Color3<float> operator *( const float a       , const Color3<float>& b ) { return Color3<float>(a) * b; }
    COMPILER_FORCEINLINE const Color3<float> operator /( const Color3<float>& a, const Color3<float>& b ) { return a * Rcp(b); }
    COMPILER_FORCEINLINE const Color3<float> operator /( const Color3<float>& a, const float b ) { return a * Rcp(b); }

    COMPILER_FORCEINLINE const Color3<float> Min( const Color3<float>& a, const Color3<float>& b ) { return _mm_min_ps(a.m128,b.m128); }
    COMPILER_FORCEINLINE const Color3<float> Max( const Color3<float>& a, const Color3<float>& b ) { return _mm_max_ps(a.m128,b.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Color3<float> operator+=(Color3<float>& a, const Color3<float>& b) { return a = a + b; }
    COMPILER_FORCEINLINE const Color3<float> operator-=(Color3<float>& a, const Color3<float>& b) { return a = a - b; }
    COMPILER_FORCEINLINE const Color3<float> operator*=(Color3<float>& a, const Color3<float>& b) { return a = a * b; }
    COMPILER_FORCEINLINE const Color3<float> operator/=(Color3<float>& a, const Color3<float>& b) { return a = a / b; }
    COMPILER_FORCEINLINE const Color3<float> operator*=(Color3<float>& a, const float b      ) { return a = a * b; }
    COMPILER_FORCEINLINE const Color3<float> operator/=(Color3<float>& a, const float b      ) { return a = a / b; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reductions
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE float ReduceAdd(const Color3<float>& v) { return v.r+v.g+v.b; }
    COMPILER_FORCEINLINE float ReduceMul(const Color3<float>& v) { return v.r*v.g*v.b; }
    COMPILER_FORCEINLINE float ReduceMin(const Color3<float>& v) { return Min(v.r,v.g,v.b); }
    COMPILER_FORCEINLINE float ReduceMax(const Color3<float>& v) { return Max(v.r,v.g,v.b); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE bool operator ==( const Color3<float>& a, const Color3<float>& b ) { return (_mm_movemask_ps(_mm_cmpeq_ps (a.m128, b.m128)) & 7) == 7; }
    COMPILER_FORCEINLINE bool operator !=( const Color3<float>& a, const Color3<float>& b ) { return (_mm_movemask_ps(_mm_cmpneq_ps(a.m128, b.m128)) & 7) != 0; }
    COMPILER_FORCEINLINE bool operator < ( const Color3<float>& a, const Color3<float>& b ) 
    {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Color3<float> Select( bool s, const Color3<float>& t, const Color3<float>& f ) 
    {
        __m128 mask = s ? _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128())) : _mm_setzero_ps();
        return _mm_blendv_ps(f, t, mask);
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Special Operators
    ////////////////////////////////////////////////////////////////////////////////

    /*! computes Luminance of a color */
    COMPILER_FORCEINLINE float Luminance (const Color3<float>& a) { return 0.212671f*a.r + 0.715160f*a.g + 0.072169f*a.b; }

    COMPILER_FORCEINLINE Color3<float> Exp (const Color3<float>& a) { return Color3<float>(exp_ps(a.m128)); }
    COMPILER_FORCEINLINE Color3<float> Log (const Color3<float>& a) { return Color3<float>(log_ps(a.m128)); }

    /*! output operator */
    inline std::ostream& operator<<(std::ostream& cout, const Color3<float>& a) 
    {
        return std::cout << "(" << a.r << ", " << a.g << ", " << a.b << ")";
    }
}
