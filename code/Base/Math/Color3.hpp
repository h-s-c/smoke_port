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
 * Color3Integer    Color3<std::uint32_t>;
 * Color3Float      Color3<float>;
 */

namespace Base
{
    ////////////////////////////////////////////////////////////////////////////////
    /// RGB Color Class
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> struct Color3
    {
        T r, g, b;

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Color3           ( )                   { }
        FORCEINLINE Color3           ( const Color3& other ) { r = other.r; g = other.g; b = other.b; }
        FORCEINLINE Color3& operator=( const Color3& other ) { r = other.r; g = other.g; b = other.b; return *this; }
        template<typename T1> FORCEINLINE explicit Color3( const Color3<T1>& a   ) : r(T(a.r)), g(T(a.g)), b(T(a.b)) {}

        FORCEINLINE explicit Color3 (const T& v)                         : r(v), g(v), b(v) {}
        FORCEINLINE explicit Color3 (const T* v, int stride = 1)         : r(v[0*stride]), g(v[1*stride]), b(v[2*stride]) {}
        FORCEINLINE          Color3 (const T& r, const T& g, const T& b) : r(r), g(g), b(b) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Color3 (ZeroTy)   : r(zero)   , g(zero)   , b(zero)    {}
        FORCEINLINE Color3 (OneTy)    : r(one)    , g(one)    , b(one)     {}
        FORCEINLINE Color3 (PosInfTy) : r(pos_inf), g(pos_inf), b(pos_inf) {}
        FORCEINLINE Color3 (NegInfTy) : r(neg_inf), g(neg_inf), b(neg_inf) {}
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE const Color3<T> operator+ (const Color3<T>& v) { return Color3<T>(+v.r,+v.g,+v.b); }
    template<typename T> FORCEINLINE const Color3<T> operator- (const Color3<T>& v) { return Color3<T>(-v.r,-v.g,-v.b); }
    template<typename T> FORCEINLINE const Color3<T> Abs       (const Color3<T>& a) { return Color3<T>(Abs(a.r),Abs(a.g),Abs(a.b)); }
    template<typename T> FORCEINLINE const Color3<T> Rcp       (const Color3<T>& a) { return Color3<T>(Rcp(a.r),Rcp(a.g),Rcp(a.b)); }
    template<typename T> FORCEINLINE const Color3<T> Rsqrt     (const Color3<T>& a) { return Color3<T>(Rsqrt(a.r),Rsqrt(a.g),Rsqrt(a.b)); }
    template<typename T> FORCEINLINE const Color3<T> Sqrt      (const Color3<T>& a) { return Color3<T>(Sqrt(a.r),Sqrt(a.g),Sqrt(a.b)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE const Color3<T> operator+(const Color3<T>& a, const Color3<T>& b) { return Color3<T>(a.r+b.r,a.g+b.g,a.b+b.b); }
    template<typename T> FORCEINLINE const Color3<T> operator-(const Color3<T>& a, const Color3<T>& b) { return Color3<T>(a.r-b.r,a.g-b.g,a.b-b.b); }
    template<typename T> FORCEINLINE const Color3<T> operator*(const T& a,       const Color3<T>& b) { return Color3<T>(a*b.r,a*b.g,a*b.b); }
    template<typename T> FORCEINLINE const Color3<T> operator*(const Color3<T>& a, const T& b      ) { return Color3<T>(a.r*b,a.g*b,a.b*b); }
    template<typename T> FORCEINLINE const Color3<T> operator*(const Color3<T>& a, const Color3<T>& b) { return Color3<T>(a.r*b.r,a.g*b.g,a.b*b.b); }
    template<typename T> FORCEINLINE const Color3<T> operator/(const Color3<T>& a, const Color3<T>& b) { return Color3<T>(a.r/b.r,a.g/b.g,a.b/b.b); }
    template<typename T> FORCEINLINE const Color3<T> operator/(const Color3<T>& a, const T& b      ) { return Color3<T>(a.r/b,a.g/b,a.b/b); }

    template<typename T> FORCEINLINE const Color3<T> Min(const Color3<T> a, const Color3<T> b) { return Color3<T>(Min(a.r,b.r),Min(a.g,b.g),Min(a.b,b.b)); }
    template<typename T> FORCEINLINE const Color3<T> Max(const Color3<T> a, const Color3<T> b) { return Color3<T>(Max(a.r,b.r),Max(a.g,b.g),Max(a.b,b.b)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE const Color3<T> operator+=(Color3<T>& a, const Color3<T>& b) { return a = a + b; }
    template<typename T> FORCEINLINE const Color3<T> operator-=(Color3<T>& a, const Color3<T>& b) { return a = a - b; }
    template<typename T> FORCEINLINE const Color3<T> operator*=(Color3<T>& a, const Color3<T>& b) { return a = a * b; }
    template<typename T> FORCEINLINE const Color3<T> operator/=(Color3<T>& a, const Color3<T>& b) { return a = a / b; }
    template<typename T> FORCEINLINE const Color3<T> operator*=(Color3<T>& a, const T& b)       { return a = a * b; }
    template<typename T> FORCEINLINE const Color3<T> operator/=(Color3<T>& a, const T& b)       { return a = a / b; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Reduction Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE const T ReduceAdd(const Color3<T>& a) { return a.r+a.g+a.b; }
    template<typename T> FORCEINLINE const T ReduceMul(const Color3<T>& a) { return a.r*a.g*a.b; }
    template<typename T> FORCEINLINE const T ReduceMin(const Color3<T>& a) { return Min(a.r,a.g,a.b); }
    template<typename T> FORCEINLINE const T ReduceMax(const Color3<T>& a) { return Max(a.r,a.g,a.b); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE bool operator ==(const Color3<T>& a, const Color3<T>& b) { return a.r == b.r && a.g == b.g && a.b == b.b; }
    template<typename T> FORCEINLINE bool operator !=(const Color3<T>& a, const Color3<T>& b) { return a.r != b.r || a.g != b.g || a.b != b.b; }
    template<typename T> FORCEINLINE bool operator < (const Color3<T>& a, const Color3<T>& b ) 
    {
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Select
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Color3<T> Select ( bool s, const Color3<T>& t, const Color3<T>& f )
    { return Color3<T>(Select(s,t.r,f.r),Select(s,t.g,f.g),Select(s,t.b,f.b)); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Special Operators
    ////////////////////////////////////////////////////////////////////////////////

    /*! computes luMinance of a color */
    template<typename T> FORCEINLINE const T LuMinance (const Color3<T>& a) { return 0.212671f*a.r + 0.715160f*a.g + 0.072169f*a.b; }

    template<typename T> FORCEINLINE Color3<T> Exp (const Color3<T>& a) { return Color3<T>(Exp(a.r),Exp(a.g),Exp(a.b)); }
    template<typename T> FORCEINLINE Color3<T> Log (const Color3<T>& a) { return Color3<T>(Log(a.r),Log(a.g),Log(a.b)); }
    template<typename T> FORCEINLINE Color3<T> Pow (const Color3<T>& a, float e) { return Exp(Log(Max(Color3<T>(1E-10f),a))*e); }

    /*! output operator */
    template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Color3<T>& a) 
    {
        return std::cout << "(" << a.r << ", " << a.g << ", " << a.b << ")";
    }
}

#if defined(PLATFORM_ARCH_X86_64)     //!< 32 bit mode causes aligmnent issues
#   include "Base/Math/SSE/Color3.hpp" //!< comment out for not using SSE colors
#endif
