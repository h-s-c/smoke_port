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
 * Vector2Integer      Vector2<std::int32_t>;
 * Vector2Float        Vector2<float>;
 * Vector2Bool         Vector2<bool>;
 */

namespace Base
{
    ////////////////////////////////////////////////////////////////////////////////
    /// Generic 2D vector Class
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> struct Vector2
    {
        T x, y;

        typedef T Scalar;
        enum { N = 2 };

        ///////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector2                      ( )                      { }
        COMPILER_FORCEINLINE Vector2                      ( const Vector2& other ) { x = other.x; y = other.y; }
        template<typename T1> COMPILER_FORCEINLINE Vector2( const Vector2<T1>& a ) : x(T(a.x)), y(T(a.y)) {}
        template<typename T1> COMPILER_FORCEINLINE Vector2& operator =( const Vector2<T1>& other ) { x = other.x; y = other.y; return *this; }

        COMPILER_FORCEINLINE explicit Vector2(const T& a             ) : x(a), y(a) {}
        COMPILER_FORCEINLINE explicit Vector2(const T& x, const T& y ) : x(x), y(y) {}
        COMPILER_FORCEINLINE explicit Vector2(const T* const a, const ssize_t stride = 1 ) : x(a[0]), y(a[stride]) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE Vector2( ZeroTy ) : x(zero), y(zero) {}
        COMPILER_FORCEINLINE Vector2( OneTy )  : x(one),  y(one) {}
        COMPILER_FORCEINLINE Vector2( PosInfTy ) : x(pos_inf), y(pos_inf) {}
        COMPILER_FORCEINLINE Vector2 (NegInfTy ) : x(neg_inf), y(neg_inf) {}

        COMPILER_FORCEINLINE const T& operator []( const size_t axis ) const { assert(axis < 2); return (&x)[axis]; }
        COMPILER_FORCEINLINE       T& operator []( const size_t axis )       { assert(axis < 2); return (&x)[axis]; }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator +( const Vector2<T>& a ) { return Vector2<T>(+a.x, +a.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator -( const Vector2<T>& a ) { return Vector2<T>(-a.x, -a.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> Abs       ( const Vector2<T>& a ) { return Vector2<T>(Abs  (a.x), Abs  (a.y)); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> Rcp       ( const Vector2<T>& a ) { return Vector2<T>(Rcp  (a.x), Rcp  (a.y)); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> Rsqrt     ( const Vector2<T>& a ) { return Vector2<T>(Rsqrt(a.x), Rsqrt(a.y)); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> Sqrt      ( const Vector2<T>& a ) { return Vector2<T>(Sqrt (a.x), Sqrt (a.y)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator +( const Vector2<T>& a, const Vector2<T>& b ) { return Vector2<T>(a.x + b.x, a.y + b.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator -( const Vector2<T>& a, const Vector2<T>& b ) { return Vector2<T>(a.x - b.x, a.y - b.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator *( const Vector2<T>& a, const Vector2<T>& b ) { return Vector2<T>(a.x * b.x, a.y * b.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator *( const       T& a, const Vector2<T>& b )    { return Vector2<T>(a   * b.x, a   * b.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator *( const Vector2<T>& a, const       T& b )    { return Vector2<T>(a.x * b  , a.y * b  ); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator /( const Vector2<T>& a, const Vector2<T>& b ) { return Vector2<T>(a.x / b.x, a.y / b.y); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator /( const Vector2<T>& a, const       T& b )    { return Vector2<T>(a.x / b  , a.y / b  ); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> operator /( const       T& a, const Vector2<T>& b )    { return Vector2<T>(a   / b.x, a   / b.y); }

    template<typename T> COMPILER_FORCEINLINE Vector2<T> Min( const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Min(a.x, b.x), Min(a.y, b.y)); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T> Max( const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Max(a.x, b.x), Max(a.y, b.y)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE Vector2<T>& operator +=( Vector2<T>& a, const Vector2<T>& b ) { a.x += b.x; a.y += b.y; return a; }
    template<typename T> COMPILER_FORCEINLINE Vector2<T>& operator -=( Vector2<T>& a, const Vector2<T>& b ) { a.x -= b.x; a.y -= b.y; return a; }
    template<typename T> COMPILER_FORCEINLINE Vector2<T>& operator *=( Vector2<T>& a, const        T& b )   { a.x *= b  ; a.y *= b  ; return a; }
    template<typename T> COMPILER_FORCEINLINE Vector2<T>& operator /=( Vector2<T>& a, const        T& b )   { a.x /= b  ; a.y /= b  ; return a; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reduction Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE T ReduceAdd( const Vector2<T>& a ) { return a.x + a.y; }
    template<typename T> COMPILER_FORCEINLINE T ReduceMul( const Vector2<T>& a ) { return a.x * a.y; }
    template<typename T> COMPILER_FORCEINLINE T ReduceMin( const Vector2<T>& a ) { return Min(a.x, a.y); }
    template<typename T> COMPILER_FORCEINLINE T ReduceMax( const Vector2<T>& a ) { return Max(a.x, a.y); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE bool operator ==( const Vector2<T>& a, const Vector2<T>& b ) { return a.x == b.x && a.y == b.y; }
    template<typename T> COMPILER_FORCEINLINE bool operator !=( const Vector2<T>& a, const Vector2<T>& b ) { return a.x != b.x || a.y != b.y; }
    template<typename T> COMPILER_FORCEINLINE bool operator < ( const Vector2<T>& a, const Vector2<T>& b ) 
    {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Euclidian Space Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE T             DotProduct  ( const Vector2<T>& a, const Vector2<T>& b ) { return a.x*b.x + a.y*b.y; }
    template<typename T> COMPILER_FORCEINLINE T             Length      ( const Vector2<T>& a )                      { return Sqrt(Dot(a,a)); }
    template<typename T> COMPILER_FORCEINLINE Vector2<T>    Normalize   ( const Vector2<T>& a )                      { return a*Rsqrt(Dot(a,a)); }
    template<typename T> COMPILER_FORCEINLINE T             Distance    ( const Vector2<T>& a, const Vector2<T>& b ) { return Length(a-b); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE Vector2<T> Select ( bool s, const Vector2<T>& t, const Vector2<T>& f ) 
    {
        return Vector2<T>(Select(s,t.x,f.x),Select(s,t.y,f.y));
    }

    template<typename T> COMPILER_FORCEINLINE Vector2<T> Select ( const typename T::Mask& s, const Vector2<T>& t, const Vector2<T>& f ) 
    {
        return Vector2<T>(Select(s,t.x,f.x),Select(s,t.y,f.y));
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Vector2<T>& a) 
    {
        return std::cout << "(" << a.x << ", " << a.y << ")";
    }
}
