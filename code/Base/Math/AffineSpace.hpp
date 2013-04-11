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
#include "Base/Math/LinearSpace3.hpp"
#include "Base/Math/Quaternion.hpp"

#include <iostream>

/* Usage:
 * AffineSpace3Float        AffineSpace<LinearSpace3<float>>;
 * AffineSpace3Double       AffineSpace<LinearSpace3<double>>;
 * OrthonormalSpace3Float   AffineSpace<Quaternion3<float>>;
 */

namespace Base
{
    #define VectorT typename L::Vector
    #define ScalarT typename L::Vector::Scalar

    ////////////////////////////////////////////////////////////////////////////////
    /// Affine Transform
    ////////////////////////////////////////////////////////////////////////////////

    template<typename L>
    struct AffineSpace
    {
        L l;           /*! linear part of affine space */
        VectorT p;     /*! affine part of affine space */

        ////////////////////////////////////////////////////////////////////////////////
        /// Constructors, Assignment, Cast, Copy Operations
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE AffineSpace           ( )                           { }
        FORCEINLINE AffineSpace           ( const AffineSpace& other ) { l = other.l; p = other.p; }
        FORCEINLINE AffineSpace& operator=( const AffineSpace& other ) { l = other.l; p = other.p; return *this; }

        FORCEINLINE AffineSpace( const VectorT& vx, const VectorT& vy, const VectorT& vz, const VectorT& p ) : l(vx,vy,vz), p(p) {}
        FORCEINLINE AffineSpace( const L& l, const VectorT& p ) : l(l), p(p) {}

        template<typename L1> FORCEINLINE explicit AffineSpace( const AffineSpace<L1>& s ) : l(s.l), p(s.p) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE AffineSpace( ZeroTy ) : l(zero), p(zero) {}
        FORCEINLINE AffineSpace( OneTy )  : l(one),  p(zero) {}

        /*! return matrix for scaling */
        static FORCEINLINE AffineSpace Scale(const VectorT& s) { return AffineSpace(L::scale(s),zero); }

        /*! return matrix for translation */
        static FORCEINLINE AffineSpace Translate(const VectorT& p) { return AffineSpace(one,p); }

        /*! return matrix for rotation around arbitrary axis */
        static FORCEINLINE AffineSpace Rotate(const VectorT& u, const ScalarT& r) { return AffineSpace(L::rotate(u,r),zero); }

        /*! return matrix for rotation around arbitrary axis and point */
        static FORCEINLINE AffineSpace Rotate(const VectorT& p, const VectorT& u, const ScalarT& r) { return translate(+p) * rotate(u,r) * translate(-p);  }

        /*! return matrix for looking at given point */
        static FORCEINLINE AffineSpace LookAtPoint(const VectorT& eye, const VectorT& point, const VectorT& up) 
        {
            VectorT Z = Normalize(point-eye);
            VectorT U = Normalize(cross(up,Z));
            VectorT V = Normalize(cross(Z,U));
            return AffineSpace(L(U,V,Z),eye);
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Unary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename L> FORCEINLINE AffineSpace<L> operator -( const AffineSpace<L>& a ) { return AffineSpace<L>(-a.l,-a.p); }
    template<typename L> FORCEINLINE AffineSpace<L> operator +( const AffineSpace<L>& a ) { return AffineSpace<L>(+a.l,+a.p); }
    template<typename L> FORCEINLINE AffineSpace<L>        Rcp( const AffineSpace<L>& a ) { L il = Rcp(a.l); return AffineSpace<L>(il,-il*a.p); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename L> FORCEINLINE const AffineSpace<L> operator +( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return AffineSpace<L>(a.l+b.l,a.p+b.p); }
    template<typename L> FORCEINLINE const AffineSpace<L> operator -( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return AffineSpace<L>(a.l-b.l,a.p-b.p); }

    template<typename L> FORCEINLINE const AffineSpace<L> operator *( const ScalarT        & a, const AffineSpace<L>& b ) { return AffineSpace<L>(a*b.l,a*b.p); }
    template<typename L> FORCEINLINE const AffineSpace<L> operator *( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return AffineSpace<L>(a.l*b.l,a.l*b.p+a.p); }
    template<typename L> FORCEINLINE const AffineSpace<L> operator /( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return a * Rcp(b); }
    template<typename L> FORCEINLINE const AffineSpace<L> operator /( const AffineSpace<L>& a, const ScalarT        & b ) { return a * Rcp(b); }

    template<typename L> FORCEINLINE AffineSpace<L>& operator *=( AffineSpace<L>& a, const AffineSpace<L>& b ) { return a = a * b; }
    template<typename L> FORCEINLINE AffineSpace<L>& operator *=( AffineSpace<L>& a, const ScalarT        & b ) { return a = a * b; }
    template<typename L> FORCEINLINE AffineSpace<L>& operator /=( AffineSpace<L>& a, const AffineSpace<L>& b ) { return a = a / b; }
    template<typename L> FORCEINLINE AffineSpace<L>& operator /=( AffineSpace<L>& a, const ScalarT        & b ) { return a = a / b; }

    template<typename L> FORCEINLINE const VectorT XfmPoint (const AffineSpace<L>& m, const VectorT& p) { return XfmPoint(m.l,p) + m.p; }
    template<typename L> FORCEINLINE const VectorT XfmVector(const AffineSpace<L>& m, const VectorT& v) { return XfmVector(m.l,v); }
    template<typename L> FORCEINLINE const VectorT XfmNormal(const AffineSpace<L>& m, const VectorT& n) { return XfmNormal(m.l,n); }

    ////////////////////////////////////////////////////////////////////////////////
    /// Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename L> FORCEINLINE bool operator ==( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return a.l == b.l && a.p == b.p; }
    template<typename L> FORCEINLINE bool operator !=( const AffineSpace<L>& a, const AffineSpace<L>& b ) { return a.l != b.l || a.p != b.p; }

    ////////////////////////////////////////////////////////////////////////////////
    /// Output Operators
    ////////////////////////////////////////////////////////////////////////////////

    template<typename L> static std::ostream& operator<<(std::ostream& cout, const AffineSpace<L>& m) 
    {
        return std::cout << "{ l = " << m.l << ", p = " << m.p << " }";
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Data conversions for AffineSpace<LinearSpace3<float>>
    ////////////////////////////////////////////////////////////////////////////////

    struct Array12f 
    {
        float values[12];
        operator float*() { return(values); }
    };

    FORCEINLINE Array12f CopyToArray(const AffineSpace<LinearSpace3<float>>& xfm)  
    {
        Array12f values;
        values[ 0] = xfm.l.vx.x;  values[ 1] = xfm.l.vx.y;  values[ 2] = xfm.l.vx.z;       
        values[ 3] = xfm.l.vy.x;  values[ 4] = xfm.l.vy.y;  values[ 5] = xfm.l.vy.z;       
        values[ 6] = xfm.l.vz.x;  values[ 7] = xfm.l.vz.y;  values[ 8] = xfm.l.vz.z;       
        values[ 9] = xfm.p.x;     values[10] = xfm.p.y;     values[11] = xfm.p.z;       
        return values;
    }

    FORCEINLINE AffineSpace<LinearSpace3<float>> CopyFromArray(const float* v) 
    {
        return AffineSpace<LinearSpace3<float>>(LinearSpace3<float>(   Vector3<float>(v[0],v[1],v[2]),
                                                                        Vector3<float>(v[3],v[4],v[5]),
                                                                        Vector3<float>(v[6],v[7],v[8])),
                                                                        Vector3<float>(v[9],v[10],v[11]));
    }

    #undef VectorT
    #undef ScalarT
}
