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
#include "Base/Math/Quaternion3.hpp"

#include <iostream>

/* Usage:
 * LinearSpace3Float     LinearSpace3<float>;
 * LinearSpace3Double    LinearSpace3<double>;
 */

namespace Base
{
    ////////////////////////////////////////////////////////////////////////////////
    /// 3D Linear Transform (3x3 Matrix)
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> struct LinearSpace3
    {
        typedef Vector3<T> Vector;

        /*! default matrix constructor */
        COMPILER_FORCEINLINE LinearSpace3           ( ) {}
        COMPILER_FORCEINLINE LinearSpace3           ( const LinearSpace3& other ) { vx = other.vx; vy = other.vy; vz = other.vz; }
        COMPILER_FORCEINLINE LinearSpace3& operator=( const LinearSpace3& other ) { vx = other.vx; vy = other.vy; vz = other.vz; return *this; }

        template<typename L1> COMPILER_FORCEINLINE explicit LinearSpace3( const LinearSpace3<L1>& s ) : vx(s.vx), vy(s.vy), vz(s.vz) {}

        /*! matrix construction from column vectors */
        COMPILER_FORCEINLINE LinearSpace3(const Vector3<T>& vx, const Vector3<T>& vy, const Vector3<T>& vz)
          : vx(vx), vy(vy), vz(vz) {}

        /*! construction from quaternion */
        COMPILER_FORCEINLINE LinearSpace3( const Quaternion<T>& q )
          : vx((q.r*q.r + q.i*q.i - q.j*q.j - q.k*q.k), 2.0f*(q.i*q.j + q.r*q.k), 2.0f*(q.i*q.k - q.r*q.j))
          , vy(2.0f*(q.i*q.j - q.r*q.k), (q.r*q.r - q.i*q.i + q.j*q.j - q.k*q.k), 2.0f*(q.j*q.k + q.r*q.i))
          , vz(2.0f*(q.i*q.k + q.r*q.j), 2.0f*(q.j*q.k - q.r*q.i), (q.r*q.r - q.i*q.i - q.j*q.j + q.k*q.k)) {}

        /*! matrix construction from row mayor data */
        COMPILER_FORCEINLINE LinearSpace3(const T& m00, const T& m01, const T& m02,
                                   const T& m10, const T& m11, const T& m12,
                                   const T& m20, const T& m21, const T& m22)
          : vx(m00,m10,m20), vy(m01,m11,m21), vz(m02,m12,m22) {}

        /*! compute the deterMinant of the matrix */
        COMPILER_FORCEINLINE const T Det() const { return Dot(vx,Cross(vy,vz)); }

        /*! compute adjoint matrix */
        COMPILER_FORCEINLINE const LinearSpace3 Adjoint() const { return LinearSpace3(Cross(vy,vz),Cross(vz,vx),Cross(vx,vy)).Transposed(); }

        /*! compute inverse matrix */
        COMPILER_FORCEINLINE const LinearSpace3 Inverse() const { return Rcp(Det())*Adjoint(); }

        /*! compute transposed matrix */
        COMPILER_FORCEINLINE const LinearSpace3 Transposed() const { return LinearSpace3<T>(vx.x,vx.y,vx.z,vy.x,vy.y,vy.z,vz.x,vz.y,vz.z); }

        /*! returns first row of matrix */
        COMPILER_FORCEINLINE const Vector3<T> Row0() const { return Vector3<T>(vx.x,vy.x,vz.x); }

        /*! returns second row of matrix */
        COMPILER_FORCEINLINE const Vector3<T> Row1() const { return Vector3<T>(vx.y,vy.y,vz.y); }

        /*! returns third row of matrix */
        COMPILER_FORCEINLINE const Vector3<T> Row2() const { return Vector3<T>(vx.z,vy.z,vz.z); }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        COMPILER_FORCEINLINE LinearSpace3( ZeroTy ) : vx(zero), vy(zero), vz(zero) {}
        COMPILER_FORCEINLINE LinearSpace3( OneTy ) : vx(one, zero, zero), vy(zero, one, zero), vz(zero, zero, one) {}

        /*! return matrix for scaling */
        static COMPILER_FORCEINLINE LinearSpace3 Scale(const Vector3<T>& s) 
        {
            return LinearSpace3(s.x,   0,   0,
                              0  , s.y,   0,
                              0  ,   0, s.z);
        }

        /*! return matrix for rotation around arbitrary axis */
        static COMPILER_FORCEINLINE LinearSpace3 Rotate(const Vector3<T>& _u, const T& r) 
        {
            Vector3<T> u = Normalize(_u);
            T s = Sin(r), c = Cos(r);
            return LinearSpace3(u.x*u.x+(1-u.x*u.x)*c,  u.x*u.y*(1-c)-u.z*s,    u.x*u.z*(1-c)+u.y*s,
                              u.x*u.y*(1-c)+u.z*s,    u.y*u.y+(1-u.y*u.y)*c,  u.y*u.z*(1-c)-u.x*s,
                              u.x*u.z*(1-c)-u.y*s,    u.y*u.z*(1-c)+u.x*s,    u.z*u.z+(1-u.z*u.z)*c);
        }

        public:

        /*! the column vectors of the matrix */
        Vector3<T> vx,vy,vz;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator -( const LinearSpace3<T>& a ) { return LinearSpace3<T>(-a.vx,-a.vy,-a.vz); }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator +( const LinearSpace3<T>& a ) { return LinearSpace3<T>(+a.vx,+a.vy,+a.vz); }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> Rcp       ( const LinearSpace3<T>& a ) { return a.Inverse(); }

    /* constructs a coordinate frame form a normal */
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> Frame(const Vector3<T>& N)
    {
        Vector3<T> dx0 = Cross(Vector3<T>(T(one),T(zero),T(zero)),N);
        Vector3<T> dx1 = Cross(Vector3<T>(T(zero),T(one),T(zero)),N);
        Vector3<T> dx = Normalize(Select(Dot(dx0,dx0) > Dot(dx1,dx1),dx0,dx1));
        Vector3<T> dy = Normalize(Cross(N,dx));
        return LinearSpace3<T>(dx,dy,N);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator +( const LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return LinearSpace3<T>(a.vx+b.vx,a.vy+b.vy,a.vz+b.vz); }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator -( const LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return LinearSpace3<T>(a.vx-b.vx,a.vy-b.vy,a.vz-b.vz); }

    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator*(const T              & a, const LinearSpace3<T>& b) { return LinearSpace3<T>(a*b.vx, a*b.vy, a*b.vz); }
    template<typename T> COMPILER_FORCEINLINE Vector3<T>         operator*(const LinearSpace3<T>& a, const Vector3<T>        & b) { return b.x*a.vx + b.y*a.vy + b.z*a.vz; }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator*(const LinearSpace3<T>& a, const LinearSpace3<T>& b) { return LinearSpace3<T>(a*b.vx, a*b.vy, a*b.vz); }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator/(const LinearSpace3<T>& a, const LinearSpace3<T>& b) { return a * Rcp(b); }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T> operator/(const LinearSpace3<T>& a, const T              & b) { return a * Rcp(b); }

    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T>& operator *=( LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return a = a * b; }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T>& operator *=( LinearSpace3<T>& a, const T              & b ) { return a = a * b; }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T>& operator /=( LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return a = a / b; }
    template<typename T> COMPILER_FORCEINLINE LinearSpace3<T>& operator /=( LinearSpace3<T>& a, const T              & b ) { return a = a / b; }

    template<typename T> COMPILER_FORCEINLINE Vector3<T> XfmPoint (const LinearSpace3<T>& s, const Vector3<T>& a) { return a.x*s.vx + a.y*s.vy + a.z*s.vz; }
    template<typename T> COMPILER_FORCEINLINE Vector3<T> XfmVector(const LinearSpace3<T>& s, const Vector3<T>& a) { return a.x*s.vx + a.y*s.vy + a.z*s.vz; }
    template<typename T> COMPILER_FORCEINLINE Vector3<T> XfmNormal(const LinearSpace3<T>& s, const Vector3<T>& a) { return XfmVector(s.Inverse().Transposed(),a); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> COMPILER_FORCEINLINE bool operator ==( const LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return a.vx == b.vx && a.vy == b.vy && a.vz == b.vz; }
    template<typename T> COMPILER_FORCEINLINE bool operator !=( const LinearSpace3<T>& a, const LinearSpace3<T>& b ) { return a.vx != b.vx || a.vy != b.vy || a.vz != b.vz; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename T> static std::ostream& operator<<(std::ostream& cout, const LinearSpace3<T>& m) 
    {
        return std::cout << "{ vx = " << m.vx << ", vy = " << m.vy << ", vz = " << m.vz << "}";
    }
}
