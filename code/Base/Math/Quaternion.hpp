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
#include "Base/Math/Vector3.hpp"

#include <iostream>

/* Usage:
 * QuaternionFloat      Quaternion<float>;
 * QuaternionDouble     Quaternion<double>;
 */

namespace Base
{
    ////////////////////////////////////////////////////////////////
    // Quaternion Struct
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct Quaternion
    {
        typedef Vector3<T> Vector;

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Quaternion          ( void )                     { }
        FORCEINLINE Quaternion          ( const Quaternion& other ) { r = other.r; i = other.i; j = other.j; k = other.k; }
        FORCEINLINE Quaternion& operator=( const Quaternion& other ) { r = other.r; i = other.i; j = other.j; k = other.k; return *this; }

        FORCEINLINE          Quaternion( const T& r        ) : r(r), i(zero), j(zero), k(zero) {}
        FORCEINLINE explicit Quaternion( const Vector3<T>& v ) : r(zero), i(v.x), j(v.y), k(v.z) {}
        FORCEINLINE          Quaternion( const T& r, const T& i, const T& j , const T& k ) : r(r), i(i), j(j), k(k) {}
        FORCEINLINE          Quaternion( const T& r, const Vector3<T>& v ) : r(r), i(v.x), j(v.y), k(v.z) {}

        INLINE Quaternion( const Vector3<T>& vx, const Vector3<T>& vy, const Vector3<T>& vz );
        INLINE Quaternion( const T& yaw, const T& pitch, const T& roll );

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE Quaternion( ZeroTy ) : r(zero), i(zero), j(zero), k(zero) {}
        FORCEINLINE Quaternion( OneTy  ) : r( one), i(zero), j(zero), k(zero) {}

        /*! return quaternion for rotation around arbitrary axis */
        static FORCEINLINE QuaternionRotate(const Vector3<T>& u, const T& r) 
        {
            return Quaternion<T>(Cos(T(0.5)*r),Sin(T(0.5)*r)*Normalize(u));
        }

        /*! returns the rotation axis of the quaternion as a vector */
        FORCEINLINE const Vector3<T> v( ) const { return Vector3<T>(i, j, k); }

      public:
        T r, i, j, k;
    };

    template<typename T> FORCEINLINE Quaternion<T> operator *( const T             & a, const Quaternion<T>& b ) { return Quaternion<T>(a * b.r, a * b.i, a * b.j, a * b.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator *( const Quaternion<T>& a, const T             & b ) { return Quaternion<T>(a.r * b, a.i * b, a.j * b, a.k * b); }

    ////////////////////////////////////////////////////////////////
    // Unary Operators
    ////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Quaternion<T> operator +( const Quaternion<T>& a ) { return Quaternion<T>(+a.r, +a.i, +a.j, +a.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator -( const Quaternion<T>& a ) { return Quaternion<T>(-a.r, -a.i, -a.j, -a.k); }
    template<typename T> FORCEINLINE Quaternion<T> Conjoint  ( const Quaternion<T>& a ) { return Quaternion<T>(a.r, -a.i, -a.j, -a.k); }
    template<typename T> FORCEINLINE T              Abs       ( const Quaternion<T>& a ) { return Sqrt(a.r*a.r + a.i*a.i + a.j*a.j + a.k*a.k); }
    template<typename T> FORCEINLINE Quaternion<T> Rcp       ( const Quaternion<T>& a ) { return conj(a)*Rcp(a.r*a.r + a.i*a.i + a.j*a.j + a.k*a.k); }
    template<typename T> FORCEINLINE Quaternion<T> Normalize ( const Quaternion<T>& a ) { return a*Rsqrt(a.r*a.r + a.i*a.i + a.j*a.j + a.k*a.k); }

    ////////////////////////////////////////////////////////////////
    // Binary Operators
    ////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE Quaternion<T> operator +( const T             & a, const Quaternion<T>& b ) { return Quaternion<T>(a + b.r,  b.i,  b.j,  b.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator +( const Quaternion<T>& a, const T             & b ) { return Quaternion<T>(a.r + b, a.i, a.j, a.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator +( const Quaternion<T>& a, const Quaternion<T>& b ) { return Quaternion<T>(a.r + b.r, a.i + b.i, a.j + b.j, a.k + b.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator -( const T             & a, const Quaternion<T>& b ) { return Quaternion<T>(a - b.r, -b.i, -b.j, -b.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator -( const Quaternion<T>& a, const T             & b ) { return Quaternion<T>(a.r - b, a.i, a.j, a.k); }
    template<typename T> FORCEINLINE Quaternion<T> operator -( const Quaternion<T>& a, const Quaternion<T>& b ) { return Quaternion<T>(a.r - b.r, a.i - b.i, a.j - b.j, a.k - b.k); }

    template<typename T> FORCEINLINE Vector3<T>       operator *( const Quaternion<T>& a, const Vector3<T>      & b ) { return (a*Quaternion<T>(b)*Conjoint(a)).v(); }
    template<typename T> FORCEINLINE Quaternion<T> operator *( const Quaternion<T>& a, const Quaternion<T>& b ) 
    {
        return Quaternion<T>(a.r*b.r - a.i*b.i - a.j*b.j - a.k*b.k,
                              a.r*b.i + a.i*b.r + a.j*b.k - a.k*b.j,
                              a.r*b.j - a.i*b.k + a.j*b.r + a.k*b.i,
                              a.r*b.k + a.i*b.j - a.j*b.i + a.k*b.r);
    }
    template<typename T> FORCEINLINE Quaternion<T> operator /( const T             & a, const Quaternion<T>& b ) { return a*Rcp(b); }
    template<typename T> FORCEINLINE Quaternion<T> operator /( const Quaternion<T>& a, const T             & b ) { return a*Rcp(b); }
    template<typename T> FORCEINLINE Quaternion<T> operator /( const Quaternion<T>& a, const Quaternion<T>& b ) { return a*Rcp(b); }

    template<typename T> FORCEINLINE Quaternion<T>& operator +=( Quaternion<T>& a, const T             & b ) { return a = a+b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator +=( Quaternion<T>& a, const Quaternion<T>& b ) { return a = a+b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator -=( Quaternion<T>& a, const T             & b ) { return a = a-b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator -=( Quaternion<T>& a, const Quaternion<T>& b ) { return a = a-b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator *=( Quaternion<T>& a, const T             & b ) { return a = a*b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator *=( Quaternion<T>& a, const Quaternion<T>& b ) { return a = a*b; }
    template<typename T> FORCEINLINE Quaternion<T>& operator /=( Quaternion<T>& a, const T             & b ) { return a = a*Rcp(b); }
    template<typename T> FORCEINLINE Quaternion<T>& operator /=( Quaternion<T>& a, const Quaternion<T>& b ) { return a = a*Rcp(b); }

    template<typename T> FORCEINLINE Vector3<T> XfmPoint ( const Quaternion<T>& a, const Vector3<T>&       b ) { return (a*Quaternion<T>(b)*Conjoint(a)).v(); }
    template<typename T> FORCEINLINE Vector3<T> XfmVector( const Quaternion<T>& a, const Vector3<T>&       b ) { return (a*Quaternion<T>(b)*Conjoint(a)).v(); }
    template<typename T> FORCEINLINE Vector3<T> XfmNormal( const Quaternion<T>& a, const Vector3<T>&       b ) { return (a*Quaternion<T>(b)*Conjoint(a)).v(); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> FORCEINLINE bool operator ==( const Quaternion<T>& a, const Quaternion<T>& b ) { return a.r == b.r && a.i == b.i && a.j == b.j && a.k == b.k; }
    template<typename T> FORCEINLINE bool operator !=( const Quaternion<T>& a, const Quaternion<T>& b ) { return a.r != b.r || a.i != b.i || a.j != b.j || a.k != b.k; }


    ////////////////////////////////////////////////////////////////////////////////
    /// Orientation Functions
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> Quaternion<T>::Quaternion( const Vector3<T>& vx, const Vector3<T>& vy, const Vector3<T>& vz )
    {
        if ( vx.x + vy.y + vz.z >= T(zero) )
        {
            const T t = T(one) + (vx.x + vy.y + vz.z);
            const T s = Rsqrt(t)*T(0.5f);
            r = t*s;
            i = (vy.z - vz.y)*s;
            j = (vz.x - vx.z)*s;
            k = (vx.y - vy.x)*s;
        }
        else if ( vx.x >= Max(vy.y, vz.z) )
        {
            const T t = (T(one) + vx.x) - (vy.y + vz.z);
            const T s = Rsqrt(t)*T(0.5f);
            r = (vy.z - vz.y)*s;
            i = t*s;
            j = (vx.y + vy.x)*s;
            k = (vz.x + vx.z)*s;
        }
        else if ( vy.y >= vz.z ) // if ( vy.y >= Max(vz.z, vx.x) )
        {
            const T t = (T(one) + vy.y) - (vz.z + vx.x);
            const T s = Rsqrt(t)*T(0.5f);
            r = (vz.x - vx.z)*s;
            i = (vx.y + vy.x)*s;
            j = t*s;
            k = (vy.z + vz.y)*s;
        }
        else //if ( vz.z >= Max(vy.y, vx.x) )
        {
            const T t = (T(one) + vz.z) - (vx.x + vy.y);
            const T s = Rsqrt(t)*T(0.5f);
            r = (vx.y - vy.x)*s;
            i = (vz.x + vx.z)*s;
            j = (vy.z + vz.y)*s;
            k = t*s;
        }
    }

    template<typename T> Quaternion<T>::Quaternion( const T& yaw, const T& pitch, const T& roll )
    {
        const T cya = Cos(yaw  *T(0.5f));
        const T cpi = Cos(pitch*T(0.5f));
        const T cro = Cos(roll *T(0.5f));
        const T sya = Sin(yaw  *T(0.5f));
        const T spi = Sin(pitch*T(0.5f));
        const T sro = Sin(roll *T(0.5f));
        r = cro*cya*cpi + sro*sya*spi;
        i = cro*cya*spi + sro*sya*cpi;
        j = cro*sya*cpi - sro*cya*spi;
        k = sro*cya*cpi - cro*sya*spi;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> static std::ostream& operator<<(std::ostream& cout, const Quaternion<T>& q) 
    {
        return std::cout << "{ r = " << q.r << ", i = " << q.i << ", j = " << q.j << ", k = " << q.k << " }";
    }
}