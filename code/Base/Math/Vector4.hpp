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
 * Vector4Integer      Vector4<std::int32_t>;
 * Vector4Float        Vector4<float>;
 * Vector4Bool         Vector4<bool>;
 */
 
namespace Base
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Generic 4D vector Class
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> struct Vector4
    {
        T x, y, z, w;

        typedef T Scalar;
        enum { N = 4 };

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Vector4    ( )                  { }
        FORCEINLINE Vector4    ( const Vector4& other ) { x = other.x; y = other.y; z = other.z; w = other.w; }
        template<typename T1> FORCEINLINE Vector4( const Vector4<T1>& a ) : x(T(a.x)), y(T(a.y)), z(T(a.z)), w(T(a.w)) {}
        template<typename T1> FORCEINLINE Vector4& operator =(const Vector4<T1>& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }

        FORCEINLINE explicit Vector4( const T& a                                     ) : x(a), y(a), z(a), w(a) {}
        FORCEINLINE explicit Vector4( const T& x, const T& y, const T& z, const T& w ) : x(x), y(y), z(z), w(w) {}
        FORCEINLINE explicit Vector4( const T* const a, const size_t stride = 1     ) : x(a[0]), y(a[stride]), z(a[2*stride]), w(a[3*stride]) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Vector4( ZeroTy   ) : x(zero), y(zero), z(zero), w(zero) {}
        FORCEINLINE Vector4( OneTy    ) : x(one),  y(one),  z(one),  w(one) {}
        FORCEINLINE Vector4( PosInfTy ) : x(pos_inf), y(pos_inf), z(pos_inf), w(pos_inf) {}
        FORCEINLINE Vector4( NegInfTy ) : x(neg_inf), y(neg_inf), z(neg_inf), w(neg_inf) {}

        FORCEINLINE const T& operator []( const size_t axis ) const { assert(axis < 4); return (&x)[axis]; }
        FORCEINLINE       T& operator []( const size_t axis )       { assert(axis < 4); return (&x)[axis]; }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector4<T> operator +( const Vector4<T>& a ) { return Vector4<T>(+a.x, +a.y, +a.z, +a.w); }
    template<typename T> FORCEINLINE Vector4<T> operator -( const Vector4<T>& a ) { return Vector4<T>(-a.x, -a.y, -a.z, -a.w); }
    template<typename T> FORCEINLINE Vector4<T> Abs       ( const Vector4<T>& a ) { return Vector4<T>(Abs  (a.x), Abs  (a.y), Abs  (a.z), Abs  (a.w)); }
    template<typename T> FORCEINLINE Vector4<T> Rcp       ( const Vector4<T>& a ) { return Vector4<T>(Rcp  (a.x), Rcp  (a.y), Rcp  (a.z), Rcp  (a.w)); }
    template<typename T> FORCEINLINE Vector4<T> Rsqrt     ( const Vector4<T>& a ) { return Vector4<T>(Rsqrt(a.x), Rsqrt(a.y), Rsqrt(a.z), Rsqrt(a.w)); }
    template<typename T> FORCEINLINE Vector4<T> Sqrt      ( const Vector4<T>& a ) { return Vector4<T>(Sqrt (a.x), Sqrt (a.y), Sqrt (a.z), Sqrt (a.w)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector4<T> operator +( const Vector4<T>& a, const Vector4<T>& b ) { return Vector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
    template<typename T> FORCEINLINE Vector4<T> operator -( const Vector4<T>& a, const Vector4<T>& b ) { return Vector4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
    template<typename T> FORCEINLINE Vector4<T> operator *( const Vector4<T>& a, const Vector4<T>& b ) { return Vector4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
    template<typename T> FORCEINLINE Vector4<T> operator *( const       T& a, const Vector4<T>& b ) { return Vector4<T>(a   * b.x, a   * b.y, a   * b.z, a   * b.w); }
    template<typename T> FORCEINLINE Vector4<T> operator *( const Vector4<T>& a, const       T& b ) { return Vector4<T>(a.x * b  , a.y * b  , a.z * b  , a.w * b  ); }
    template<typename T> FORCEINLINE Vector4<T> operator /( const Vector4<T>& a, const Vector4<T>& b ) { return Vector4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
    template<typename T> FORCEINLINE Vector4<T> operator /( const Vector4<T>& a, const       T& b ) { return Vector4<T>(a.x / b  , a.y / b  , a.z / b  , a.w / b  ); }
    template<typename T> FORCEINLINE Vector4<T> operator /( const       T& a, const Vector4<T>& b ) { return Vector4<T>(a   / b.x, a   / b.y, a   / b.z, a   / b.w); }

    template<typename T> FORCEINLINE Vector4<T> Min(const Vector4<T>& a, const Vector4<T>& b) { return Vector4<T>(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z), Min(a.w, b.w)); }
    template<typename T> FORCEINLINE Vector4<T> Max(const Vector4<T>& a, const Vector4<T>& b) { return Vector4<T>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z), Max(a.w, b.w)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector4<T>& operator +=( Vector4<T>& a, const Vector4<T>& b ) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a; }
    template<typename T> FORCEINLINE Vector4<T>& operator -=( Vector4<T>& a, const Vector4<T>& b ) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a; }
    template<typename T> FORCEINLINE Vector4<T>& operator *=( Vector4<T>& a, const       T& b ) { a.x *= b  ; a.y *= b  ; a.z *= b  ; a.w *= b  ; return a; }
    template<typename T> FORCEINLINE Vector4<T>& operator /=( Vector4<T>& a, const       T& b ) { a.x /= b  ; a.y /= b  ; a.z /= b  ; a.w /= b  ; return a; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reduction Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE T ReduceAdd( const Vector4<T>& a ) { return a.x + a.y + a.z + a.w; }
    template<typename T> FORCEINLINE T ReduceMul( const Vector4<T>& a ) { return a.x * a.y * a.z * a.w; }
    template<typename T> FORCEINLINE T ReduceMin( const Vector4<T>& a ) { return Min(a.x, a.y, a.z, a.w); }
    template<typename T> FORCEINLINE T ReduceMax( const Vector4<T>& a ) { return Max(a.x, a.y, a.z, a.w); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE bool operator ==( const Vector4<T>& a, const Vector4<T>& b ) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
    template<typename T> FORCEINLINE bool operator !=( const Vector4<T>& a, const Vector4<T>& b ) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }
    template<typename T> FORCEINLINE bool operator < ( const Vector4<T>& a, const Vector4<T>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        if (a.w != b.w) return a.w < b.w;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Euclidian Space Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE T       Dot      ( const Vector4<T>& a, const Vector4<T>& b ) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
    template<typename T> FORCEINLINE T       Length   ( const Vector4<T>& a )                   { return Sqrt(Dot(a,a)); }
    template<typename T> FORCEINLINE Vector4<T> Normalize( const Vector4<T>& a )                   { return a*Rsqrt(Dot(a,a)); }
    template<typename T> FORCEINLINE T       Distance ( const Vector4<T>& a, const Vector4<T>& b ) { return Length(a-b); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Vector4<T> Select ( bool s, const Vector4<T>& t, const Vector4<T>& f ) 
    {
        return Vector4<T>(Select(s,t.x,f.x),Select(s,t.y,f.y),Select(s,t.z,f.z),Select(s,t.w,f.w));
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Vector4<T>& a) 
    {
        return std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
    }
}
