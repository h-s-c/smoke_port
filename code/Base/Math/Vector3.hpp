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
#include "Base/Math/Constants.hpp"
#include "Base/Math/Math.hpp"

#include <iostream>

/* Usage:
 * Vector3Integer      Vector3<std::int32_t>;
 * Vector3Float        Vector3<float>;
 * Vector3Bool         Vector3<bool>;
 */

namespace Base
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Generic 3D vector Class
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> struct Vector3
    {
        T x, y, z;

        typedef T Scalar;
        enum { N  = 3 };

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Vector3     ( )                  { }
        FORCEINLINE Vector3     ( const Vector3& other ) { x = other.x; y = other.y; z = other.z; }
        template<typename T1> FORCEINLINE Vector3( const Vector3<T1>& a ) : x(T(a.x)), y(T(a.y)), z(T(a.z)) {}
        template<typename T1> FORCEINLINE Vector3& operator =(const Vector3<T1>& other) { x = other.x; y = other.y; z = other.z; return *this; }

        FORCEINLINE explicit Vector3( const T& a                         ) : x(a), y(a), z(a) {}
        FORCEINLINE explicit Vector3( const T& x, const T& y, const T& z ) : x(x), y(y), z(z) {}
        FORCEINLINE explicit Vector3( const T* const a, const size_t stride = 1 ) : x(a[0]), y(a[stride]), z(a[2*stride]) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Vector3( ZeroTy   ) : x(zero), y(zero), z(zero) {}
        FORCEINLINE Vector3( OneTy    ) : x(one),  y(one),  z(one) {}
        FORCEINLINE Vector3( PosInfTy ) : x(pos_inf), y(pos_inf), z(pos_inf) {}
        FORCEINLINE Vector3( NegInfTy ) : x(neg_inf), y(neg_inf), z(neg_inf) {}

        FORCEINLINE const T& operator []( const size_t axis ) const { assert(axis < 3); return (&x)[axis]; }
        FORCEINLINE       T& operator []( const size_t axis )       { assert(axis < 3); return (&x)[axis]; }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector3<T> operator +( const Vector3<T>& a ) { return Vector3<T>(+a.x, +a.y, +a.z); }
    template<typename T> FORCEINLINE Vector3<T> operator -( const Vector3<T>& a ) { return Vector3<T>(-a.x, -a.y, -a.z); }
    template<typename T> FORCEINLINE Vector3<T> Abs       ( const Vector3<T>& a ) { return Vector3<T>(Abs  (a.x), Abs  (a.y), Abs  (a.z)); }
    template<typename T> FORCEINLINE Vector3<T> Rcp       ( const Vector3<T>& a ) { return Vector3<T>(Rcp  (a.x), Rcp  (a.y), Rcp  (a.z)); }
    template<typename T> FORCEINLINE Vector3<T> Rsqrt     ( const Vector3<T>& a ) { return Vector3<T>(Rsqrt(a.x), Rsqrt(a.y), Rsqrt(a.z)); }
    template<typename T> FORCEINLINE Vector3<T> Sqrt      ( const Vector3<T>& a ) { return Vector3<T>(Sqrt (a.x), Sqrt (a.y), Sqrt (a.z)); }

    template<typename T> FORCEINLINE Vector3<T> Rcp_safe( const Vector3<T>& a ) 
    {
        return Vector3<T>(Select(a.x==0.0f,T(std::numeric_limits<float>::Max()),Rcp(a.x)),
                   Select(a.y==0.0f,T(std::numeric_limits<float>::Max()),Rcp(a.y)),
                   Select(a.z==0.0f,T(std::numeric_limits<float>::Max()),Rcp(a.z)));
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector3<T> operator +( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z); }
    template<typename T> FORCEINLINE Vector3<T> operator -( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z); }
    template<typename T> FORCEINLINE Vector3<T> operator *( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z); }
    template<typename T> FORCEINLINE Vector3<T> operator *( const       T& a, const Vector3<T>& b ) { return Vector3<T>(a   * b.x, a   * b.y, a   * b.z); }
    template<typename T> FORCEINLINE Vector3<T> operator *( const Vector3<T>& a, const       T& b ) { return Vector3<T>(a.x * b  , a.y * b  , a.z * b  ); }
    template<typename T> FORCEINLINE Vector3<T> operator /( const Vector3<T>& a, const       T& b ) { return Vector3<T>(a.x / b  , a.y / b  , a.z / b  ); }
    template<typename T> FORCEINLINE Vector3<T> operator /( const       T& a, const Vector3<T>& b ) { return Vector3<T>(a   / b.x, a   / b.y, a   / b.z); }
    template<typename T> FORCEINLINE Vector3<T> operator /( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<T>(a.x / b.x, a.y / b.y, a.z / b.z); }

    template<typename T> FORCEINLINE Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z)); }
    template<typename T> FORCEINLINE Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE const Vector3<T>& operator +=( Vector3<T>& a, const Vector3<T>& b ) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
    template<typename T> FORCEINLINE const Vector3<T>& operator -=( Vector3<T>& a, const Vector3<T>& b ) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
    template<typename T> FORCEINLINE const Vector3<T>& operator *=( Vector3<T>& a, const       T& b ) { a.x *= b  ; a.y *= b  ; a.z *= b  ; return a; }
    template<typename T> FORCEINLINE const Vector3<T>& operator /=( Vector3<T>& a, const       T& b ) { a.x /= b  ; a.y /= b  ; a.z /= b  ; return a; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reduction Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE T ReduceAdd( const Vector3<T>& a ) { return a.x + a.y + a.z; }
    template<typename T> FORCEINLINE T ReduceMul( const Vector3<T>& a ) { return a.x * a.y * a.z; }
    template<typename T> FORCEINLINE T ReduceMin( const Vector3<T>& a ) { return Min(a.x, a.y, a.z); }
    template<typename T> FORCEINLINE T ReduceMax( const Vector3<T>& a ) { return Max(a.x, a.y, a.z); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE bool operator ==( const Vector3<T>& a, const Vector3<T>& b ) { return a.x == b.x && a.y == b.y && a.z == b.z; }
    template<typename T> FORCEINLINE bool operator !=( const Vector3<T>& a, const Vector3<T>& b ) { return a.x != b.x || a.y != b.y || a.z != b.z; }
    template<typename T> FORCEINLINE bool operator < ( const Vector3<T>& a, const Vector3<T>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Euclidian Space Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE T       Dot      ( const Vector3<T>& a, const Vector3<T>& b ) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    template<typename T> FORCEINLINE T       Length   ( const Vector3<T>& a )                   { return Sqrt(Dot(a,a)); }
    template<typename T> FORCEINLINE Vector3<T> Normalize( const Vector3<T>& a )                   { return a*Rsqrt(Dot(a,a)); }
    template<typename T> FORCEINLINE T       Distance ( const Vector3<T>& a, const Vector3<T>& b ) { return Length(a-b); }
    template<typename T> FORCEINLINE Vector3<T> cross    ( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<T>(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector3<T> Select ( bool s, const Vector3<T>& t, const Vector3<T>& f ) 
    {
        return Vector3<T>(Select(s,t.x,f.x),Select(s,t.y,f.y),Select(s,t.z,f.z));
    }

    template<typename T> FORCEINLINE Vector3<T> Select ( const typename T::Mask& s, const Vector3<T>& t, const Vector3<T>& f ) 
    {
        return Vector3<T>(Select(s,t.x,f.x),Select(s,t.y,f.y),Select(s,t.z,f.z));
    }

    template<typename T> FORCEINLINE int MaxDim ( const Vector3<T>& a ) 
    { 
        if (a.x > a.y) 
        {
            if (a.x > a.z) 
                return 0; 
            else 
                return 2;
        }
        else 
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

    template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Vector3<T>& a) 
    {
        return std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ")";
    }
}

//!< use SSE for short vectors only in 64 bit mode (32 bit mode causes alignment issues)
#if defined(PLATFORM_ARCH_X86_64) 
#   include "Base/Math/SSE/Vector3.hpp"    //!< comment out for not using SSE vectors
#endif

////////////////////////////////////////////////////////////////////////////////
/// Comparison Operators
////////////////////////////////////////////////////////////////////////////////
namespace Base
{
    template<typename T> FORCEINLINE Vector3<bool> EqMask( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x==b.x,a.y==b.y,a.z==b.z); }
    template<typename T> FORCEINLINE Vector3<bool> NeqMask(const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x!=b.x,a.y!=b.y,a.z!=b.z); }
    template<typename T> FORCEINLINE Vector3<bool> LtMask( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x< b.x,a.y< b.y,a.z< b.z); }
    template<typename T> FORCEINLINE Vector3<bool> LeMask( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x<=b.x,a.y<=b.y,a.z<=b.z); }
    template<typename T> FORCEINLINE Vector3<bool> GtMask( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x> b.x,a.y> b.y,a.z> b.z); }
    template<typename T> FORCEINLINE Vector3<bool> GeMask( const Vector3<T>& a, const Vector3<T>& b ) { return Vector3<bool>(a.x>=b.x,a.y>=b.y,a.z>=b.z); }
}
