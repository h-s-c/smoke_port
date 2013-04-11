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

/* Usage: see Base/Math/Vector3.hpp
 */

namespace Base
{
////////////////////////////////////////////////////////////////////////////////////////////////////
/// SSE Vector3<float> Type
////////////////////////////////////////////////////////////////////////////////////////////////////

    template<> struct COMPILER_ALIGN(16) Vector3<float>
    {
        union 
        {
            __m128 m128;
            struct { float x,y,z; int a; };
        };

        typedef float Scalar;
        enum { N = 3 };

        ////////////////////////////////////////////////////////////////////////////////
        /// Constructors, Assignment & Cast Operators
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( ) {}
        COMPILER_FORCEINLINE Vector3( const __m128 a ) : m128(a) {}
        COMPILER_FORCEINLINE Vector3( const Vector3<float>& other ) : m128(other.m128) {}
        COMPILER_FORCEINLINE Vector3<float>& operator =(const Vector3<float>& other) { m128 = other.m128; return *this; }

        COMPILER_FORCEINLINE explicit Vector3( const float a ) : m128(_mm_set1_ps(a)) {}
        COMPILER_FORCEINLINE explicit Vector3( const float x, const float y, const float z) : m128(_mm_set_ps(z, z, y, x)) {}
        COMPILER_FORCEINLINE explicit Vector3( const float* const a, const ssize_t stride = 1 ) : m128(_mm_set_ps(0.0f,a[2*stride],a[stride],a[0])) {}
        COMPILER_FORCEINLINE explicit Vector3( const __m128i a ) : m128(_mm_cvtepi32_ps(a)) {}

        COMPILER_FORCEINLINE operator const __m128&( void ) const { return m128; }
        COMPILER_FORCEINLINE operator       __m128&( void )       { return m128; }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( ZeroTy   ) : m128(_mm_setzero_ps()) {}
        COMPILER_FORCEINLINE Vector3( OneTy    ) : m128(_mm_set1_ps(1.0f)) {}
        COMPILER_FORCEINLINE Vector3( PosInfTy ) : m128(_mm_set1_ps(pos_inf)) {}
        COMPILER_FORCEINLINE Vector3( NegInfTy ) : m128(_mm_set1_ps(neg_inf)) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Array Access
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE const float& operator []( const size_t index ) const { assert(index < 3); return (&x)[index]; }
        COMPILER_FORCEINLINE       float& operator []( const size_t index )       { assert(index < 3); return (&x)[index]; }
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<float> operator +( const Vector3<float>& a ) { return a; }
    COMPILER_FORCEINLINE const Vector3<float> operator -( const Vector3<float>& a ) 
    {
        const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
        return _mm_xor_ps(a.m128, mask);
    }
    COMPILER_FORCEINLINE const Vector3<float> Abs  ( const Vector3<float>& a ) 
    {
        const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
        return _mm_and_ps(a.m128, mask);
    }
    COMPILER_FORCEINLINE const Vector3<float> Sign ( const Vector3<float>& a ) 
    {
    return _mm_blendv_ps(Vector3<float>(one), -Vector3<float>(one), _mm_cmplt_ps (a,Vector3<float>(zero))); 
    }
    COMPILER_FORCEINLINE const Vector3<float> Rcp  ( const Vector3<float>& a ) 
    {
        const Vector3<float> r = _mm_Rcp_ps(a.m128);
        return _mm_sub_ps(_mm_add_ps(r, r), _mm_mul_ps(_mm_mul_ps(r, r), a));
    }
    COMPILER_FORCEINLINE const Vector3<float> Sqrt ( const Vector3<float>& a ) { return _mm_sqrt_ps(a.m128); }
    COMPILER_FORCEINLINE const Vector3<float> Sqr  ( const Vector3<float>& a ) { return _mm_mul_ps(a,a); }
    COMPILER_FORCEINLINE const Vector3<float> Rsqrt( const Vector3<float>& a ) 
    {
        __m128 r = _mm_Rsqrt_ps(a.m128);
        return _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.5f),r), _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, _mm_set1_ps(-0.5f)), r), _mm_mul_ps(r, r)));
    }
    COMPILER_FORCEINLINE const Vector3<float> RcpSafe( const Vector3<float>& a ) 
    {
        __m128 mask = _mm_cmpeq_ps (a.m128, _mm_setzero_ps());

        /*! workaround for compiler bug in VS2008 */
        #if defined(COMPILER_MSVC) && (COMPILER_VERSION_MAJOR == 9)
        return _mm_or_ps(_mm_and_ps(mask, _mm_set1_ps(std::numeric_limits<float>::Max())), _mm_andnot_ps(mask, Rcp(a))); 
        #else
        return _mm_blendv_ps(Rcp(a), _mm_set1_ps(std::numeric_limits<float>::Max()), mask);
        #endif
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<float> operator +( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_add_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<float> operator -( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_sub_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<float> operator *( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_mul_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<float> operator *( const Vector3<float>& a, const float b ) { return a * Vector3<float>(b); }
    COMPILER_FORCEINLINE const Vector3<float> operator *( const float a, const Vector3<float>& b ) { return Vector3<float>(a) * b; }
    COMPILER_FORCEINLINE const Vector3<float> operator /( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_div_ps(a.m128,b.m128); }
    COMPILER_FORCEINLINE const Vector3<float> operator /( const Vector3<float>& a, const float b        ) { return _mm_div_ps(a.m128,_mm_set1_ps(b)); }
    COMPILER_FORCEINLINE const Vector3<float> operator /( const        float a, const Vector3<float>& b ) { return _mm_div_ps(_mm_set1_ps(a),b.m128); }

    COMPILER_FORCEINLINE const Vector3<float> Min( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_min_ps(a.m128,b.m128); }
    COMPILER_FORCEINLINE const Vector3<float> Max( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_max_ps(a.m128,b.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE Vector3<float>& operator +=( Vector3<float>& a, const Vector3<float>& b ) { return a = a + b; }
    COMPILER_FORCEINLINE Vector3<float>& operator -=( Vector3<float>& a, const Vector3<float>& b ) { return a = a - b; }
    COMPILER_FORCEINLINE Vector3<float>& operator *=( Vector3<float>& a, const float b ) { return a = a * b; }
    COMPILER_FORCEINLINE Vector3<float>& operator /=( Vector3<float>& a, const float b ) { return a = a / b; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reductions
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE float ReduceAdd(const Vector3<float>& v) { return v.x+v.y+v.z; }
    COMPILER_FORCEINLINE float ReduceMul(const Vector3<float>& v) { return v.x*v.y*v.z; }
    COMPILER_FORCEINLINE float ReduceMin(const Vector3<float>& v) { return Min(v.x,v.y,v.z); }
    COMPILER_FORCEINLINE float ReduceMax(const Vector3<float>& v) { return Max(v.x,v.y,v.z); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE bool operator ==( const Vector3<float>& a, const Vector3<float>& b ) { return (_mm_movemask_ps(_mm_cmpeq_ps (a.m128, b.m128)) & 7) == 7; }
    COMPILER_FORCEINLINE bool operator !=( const Vector3<float>& a, const Vector3<float>& b ) { return (_mm_movemask_ps(_mm_cmpneq_ps(a.m128, b.m128)) & 7) != 0; }
    COMPILER_FORCEINLINE bool operator < ( const Vector3<float>& a, const Vector3<float>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        return false;
    }

    COMPILER_FORCEINLINE Vector3<bool> EqMask( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmpeq_ps (a.m128, b.m128); }
    COMPILER_FORCEINLINE Vector3<bool> NeqMask(const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmpneq_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE Vector3<bool> LtMask( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmplt_ps (a.m128, b.m128); }
    COMPILER_FORCEINLINE Vector3<bool> LeMask( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmple_ps (a.m128, b.m128); }
    COMPILER_FORCEINLINE Vector3<bool> GtMask( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmpnle_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE Vector3<bool> GeMask( const Vector3<float>& a, const Vector3<float>& b ) { return _mm_cmpnlt_ps(a.m128, b.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Euclidian Space Operators
    ////////////////////////////////////////////////////////////////////////////////

    /* SSE4.1*/
    COMPILER_FORCEINLINE float Dot ( const Vector3<float>& a, const Vector3<float>& b ) 
    {
        return _mm_cvtss_f32(_mm_dp_ps(a,b,0x7F));
    }
    /*COMPILER_FORCEINLINE float Dot ( const Vector3<float>& a, const Vector3<float>& b ) 
    {
        return ReduceAdd(a*b);
    }*/

    COMPILER_FORCEINLINE Vector3<float> Cross ( const Vector3<float>& a, const Vector3<float>& b ) 
    {
        ssef a0 = shuffle<1,2,0,3>(ssef(a));
        ssef b0 = shuffle<2,0,1,3>(ssef(b));
        ssef a1 = shuffle<2,0,1,3>(ssef(a));
        ssef b1 = shuffle<1,2,0,3>(ssef(b));
        return Vector3<float>(a0*b0-a1*b1);
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<float> Select( bool s, const Vector3<float>& t, const Vector3<float>& f ) 
    {
        __m128 mask = s ? _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128())) : _mm_setzero_ps();
        return _mm_blendv_ps(f, t, mask);
    }

    COMPILER_FORCEINLINE int MaxDim ( const Vector3<float>& a ) 
    { 
        if (a.x > a.y) 
        {
            if (a.x > a.z) 
                return 0; 
            else 
                return 2;
        } else 
        {
            if (a.y > a.z) 
                return 1; 
            else 
                return 2;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    inline std::ostream& operator<<(std::ostream& cout, const Vector3<float>& a) 
    {
        return std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ")";
    }
    

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SSE Vector3<std::int32_t> Type
////////////////////////////////////////////////////////////////////////////////////////////////////

    template<> struct COMPILER_ALIGN(16) Vector3<std::int32_t>
    {
        union 
        {
            __m128i m128;
            struct { std::int32_t x,y,z; std::int32_t a; };
        };

        typedef std::int32_t Scalar;
        enum { N = 3 };

        ////////////////////////////////////////////////////////////////////////////////
        /// Constructors, Assignment & Cast Operators
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( ) {}
        COMPILER_FORCEINLINE Vector3( const __m128i a ) : m128(a) {}
        COMPILER_FORCEINLINE Vector3( const Vector3<std::int32_t>& other ) : m128(other.m128) {}
        COMPILER_FORCEINLINE Vector3<std::int32_t>& operator =(const Vector3<std::int32_t>& other) { m128 = other.m128; return *this; }

        COMPILER_FORCEINLINE explicit Vector3( const std::int32_t a ) : m128(_mm_set1_epi32(a)) {}
        COMPILER_FORCEINLINE explicit Vector3( const std::int32_t x, const std::int32_t y, const std::int32_t z) : m128(_mm_set_epi32(z, z, y, x)) {}
        COMPILER_FORCEINLINE explicit Vector3( const std::int32_t* const a, const ssize_t stride = 1 ) : m128(_mm_set_epi32(0,a[2*stride],a[stride],a[0])) {}
        COMPILER_FORCEINLINE explicit Vector3( const __m128 a ) : m128(_mm_cvtps_epi32(a)) {}

        COMPILER_FORCEINLINE operator const __m128i&( void ) const { return m128; }
        COMPILER_FORCEINLINE operator       __m128i&( void )       { return m128; }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( ZeroTy   ) : m128(_mm_setzero_si128()) {}
        COMPILER_FORCEINLINE Vector3( OneTy    ) : m128(_mm_set1_epi32(1)) {}
        COMPILER_FORCEINLINE Vector3( PosInfTy ) : m128(_mm_set1_epi32(pos_inf)) {}
        COMPILER_FORCEINLINE Vector3( NegInfTy ) : m128(_mm_set1_epi32(neg_inf)) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Array Access
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE const std::int32_t& operator []( const size_t index ) const { assert(index < 3); return (&x)[index]; }
        COMPILER_FORCEINLINE       std::int32_t& operator []( const size_t index )       { assert(index < 3); return (&x)[index]; }
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator +( const Vector3<std::int32_t>& a ) { return a; }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator -( const Vector3<std::int32_t>& a ) { return _mm_sub_epi32(_mm_setzero_si128(), a.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> Abs       ( const Vector3<std::int32_t>& a ) { return _mm_Abs_epi32(a.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator +( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_add_epi32(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator +( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a+Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator +( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a)+b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator -( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_sub_epi32(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator -( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a-Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator -( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a)-b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator *( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_mullo_epi32(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator *( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a * Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator *( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a) * b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator &( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_and_si128(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator &( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a & Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator &( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a) & b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator |( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_or_si128(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator |( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a | Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator |( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a) | b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator ^( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_xor_si128(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator ^( const Vector3<std::int32_t>& a, const std::int32_t        b ) { return a ^ Vector3<std::int32_t>(b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator ^( const std::int32_t        a, const Vector3<std::int32_t>& b ) { return Vector3<std::int32_t>(a) ^ b; }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator <<( const Vector3<std::int32_t>& a, const std::int32_t n ) { return _mm_slli_epi32(a.m128, n); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> operator >>( const Vector3<std::int32_t>& a, const std::int32_t n ) { return _mm_srai_epi32(a.m128, n); }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> Sra ( const Vector3<std::int32_t>& a, const std::int32_t b ) { return _mm_srai_epi32(a.m128, b); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> Srl ( const Vector3<std::int32_t>& a, const std::int32_t b ) { return _mm_srli_epi32(a.m128, b); }

    COMPILER_FORCEINLINE const Vector3<std::int32_t> Min( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_min_epi32(a.m128,b.m128); }
    COMPILER_FORCEINLINE const Vector3<std::int32_t> Max( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return _mm_max_epi32(a.m128,b.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator +=( Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return a = a + b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator +=( Vector3<std::int32_t>& a, const std::int32_t32&     b ) { return a = a + b; }

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator -=( Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return a = a - b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator -=( Vector3<std::int32_t>& a, const std::int32_t32&     b ) { return a = a - b; }

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator *=( Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return a = a * b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator *=( Vector3<std::int32_t>& a, const std::int32_t32&     b ) { return a = a * b; }

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator &=( Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return a = a & b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator &=( Vector3<std::int32_t>& a, const std::int32_t32&     b ) { return a = a & b; }

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator |=( Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return a = a | b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator |=( Vector3<std::int32_t>& a, const std::int32_t32&     b ) { return a = a | b; }

    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator <<=( Vector3<std::int32_t>& a, const std::int32_t32&    b ) { return a = a << b; }
    COMPILER_FORCEINLINE Vector3<std::int32_t>& operator >>=( Vector3<std::int32_t>& a, const std::int32_t32&    b ) { return a = a >> b; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reductions
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE std::int32_t ReduceAdd(const Vector3<std::int32_t>& v) { return v.x+v.y+v.z; }
    COMPILER_FORCEINLINE std::int32_t ReduceMul(const Vector3<std::int32_t>& v) { return v.x*v.y*v.z; }
    COMPILER_FORCEINLINE std::int32_t ReduceMin(const Vector3<std::int32_t>& v) { return Min(v.x,v.y,v.z); }
    COMPILER_FORCEINLINE std::int32_t ReduceMax(const Vector3<std::int32_t>& v) { return Max(v.x,v.y,v.z); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE bool operator ==( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return (_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(a.m128, b.m128))) & 7) == 7; }
    COMPILER_FORCEINLINE bool operator !=( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) { return (_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(a.m128, b.m128))) & 7) != 7; }
    COMPILER_FORCEINLINE bool operator < ( const Vector3<std::int32_t>& a, const Vector3<std::int32_t>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    inline std::ostream& operator<<(std::ostream& cout, const Vector3<std::int32_t>& a) 
    {
        return std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ")";
    }
    
    
////////////////////////////////////////////////////////////////////////////////////////////////////
/// SSE Vector3<bool> Type
////////////////////////////////////////////////////////////////////////////////////////////////////

    template<> struct COMPILER_ALIGN(16) Vector3<bool>
    {
        union 
        {
            __m128 m128;
            struct { int x,y,z; int a; };
        };

        typedef int Scalar;
        enum { N = 3 };

        ////////////////////////////////////////////////////////////////////////////////
        /// Constructors, Assignment & Cast Operators
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( ) {}
        COMPILER_FORCEINLINE Vector3( const __m128  input ) : m128(input) {}
        COMPILER_FORCEINLINE Vector3( const Vector3<bool>& other ) : m128(other.m128) {}
        COMPILER_FORCEINLINE Vector3<bool>& operator =(const Vector3<bool>& other) { m128 = other.m128; return *this; }

        COMPILER_FORCEINLINE explicit Vector3           ( bool  a )
          : m128(_mm_lookupmask_ps[(size_t(a) << 3) | (size_t(a) << 2) | (size_t(a) << 1) | size_t(a)]) {}
        COMPILER_FORCEINLINE explicit Vector3           ( bool  a, bool  b, bool  c)
          : m128(_mm_lookupmask_ps[(size_t(c) << 2) | (size_t(b) << 1) | size_t(a)]) {}

        COMPILER_FORCEINLINE operator const __m128&( void ) const { return m128; }
        COMPILER_FORCEINLINE operator       __m128&( void )       { return m128; }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector3( FalseTy ) : m128(_mm_setzero_ps()) {}
        COMPILER_FORCEINLINE Vector3( TrueTy  ) : m128(_mm_castsi128_ps(_mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128()))) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Array Access
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE const int& operator []( const size_t index ) const { assert(index < 3); return (&x)[index]; }
        COMPILER_FORCEINLINE       int& operator []( const size_t index )       { assert(index < 3); return (&x)[index]; }
    };


    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<bool> operator !( const Vector3<bool>& a ) { return _mm_xor_ps(a.m128, Vector3<bool>(True)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<bool> operator &( const Vector3<bool>& a, const Vector3<bool>& b ) { return _mm_and_ps(a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<bool> operator |( const Vector3<bool>& a, const Vector3<bool>& b ) { return _mm_or_ps (a.m128, b.m128); }
    COMPILER_FORCEINLINE const Vector3<bool> operator ^( const Vector3<bool>& a, const Vector3<bool>& b ) { return _mm_xor_ps(a.m128, b.m128); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE const Vector3<bool> operator &=( Vector3<bool>& a, const Vector3<bool>& b ) { return a = a & b; }
    COMPILER_FORCEINLINE const Vector3<bool> operator |=( Vector3<bool>& a, const Vector3<bool>& b ) { return a = a | b; }
    COMPILER_FORCEINLINE const Vector3<bool> operator ^=( Vector3<bool>& a, const Vector3<bool>& b ) { return a = a ^ b; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators + Select
    ////////////////////////////////////////////////////////////////////////////////

    COMPILER_FORCEINLINE bool operator ==( const Vector3<bool>& a, const Vector3<bool>& b ) 
    { 
        return (_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a.m128), _mm_castps_si128(b.m128)))) & 7) == 7; 
    }
    COMPILER_FORCEINLINE bool operator !=( const Vector3<bool>& a, const Vector3<bool>& b ) 
    { 
        return (_mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(a.m128), _mm_castps_si128(b.m128)))) & 7) != 7; 
    }
    COMPILER_FORCEINLINE bool operator < ( const Vector3<bool>& a, const Vector3<bool>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    inline std::ostream& operator<<(std::ostream& cout, const Vector3<bool>& a) 
    {
        return cout << "(" << (a.x ? "1" : "0") << ", " << (a.y ? "1" : "0") << ", " << (a.z ? "1" : "0") << ")";
    }
}
