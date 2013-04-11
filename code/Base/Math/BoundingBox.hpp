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
#include "Base/Math/Vector2.hpp"
#include "Base/Math/Vector3.hpp"

#include <iostream>

/* Usage:
 * BoundingBox2Float   BoundingBox<Vector2<float>>;
 * BoundingBox3Float   BoundingBox<Vector3<float>>;
 */

namespace Base
{
    template<typename T>
    struct BoundingBox
    {
        T lower, upper;

        ////////////////////////////////////////////////////////////////////////////////
        /// Construction
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE BoundingBox           ( )                   { }
        FORCEINLINE BoundingBox           ( const BoundingBox& other ) { lower = other.lower; upper = other.upper; }
        FORCEINLINE BoundingBox& operator=( const BoundingBox& other ) { lower = other.lower; upper = other.upper; return *this; }

        FORCEINLINE BoundingBox ( const T& v                 ) : lower(v),   upper(v) {}
        FORCEINLINE BoundingBox ( const T& lower, const T& upper ) : lower(lower), upper(upper) {}

        ////////////////////////////////////////////////////////////////////////////////
        /// Growing Bounds
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE void Grow(const BoundingBox& other) { lower = Min(lower,other.lower); upper = Max(upper,other.upper); }
        FORCEINLINE void Grow(const T   & other) { lower = Min(lower,other    ); upper = Max(upper,other    ); }

        /*! computes the size of the box */
        FORCEINLINE bool Empty() const { for (int i=0; i<T::N; i++) if (lower[i] > upper[i]) return true; return false; }

        /*! computes the size of the box */
        FORCEINLINE T Size() const { return upper - lower; }

        ////////////////////////////////////////////////////////////////////////////////
        /// Constants
        ////////////////////////////////////////////////////////////////////////////////

        FORCEINLINE BoundingBox( EmptyTy ) : lower(pos_inf), upper(neg_inf) {}
        FORCEINLINE BoundingBox( FullTy  ) : lower(neg_inf), upper(pos_inf) {}
        FORCEINLINE BoundingBox( FalseTy ) : lower(pos_inf), upper(neg_inf) {}
        FORCEINLINE BoundingBox( TrueTy  ) : lower(neg_inf), upper(pos_inf) {}
        FORCEINLINE BoundingBox( NegInfTy ): lower(pos_inf), upper(neg_inf) {}
        FORCEINLINE BoundingBox( PosInfTy ): lower(neg_inf), upper(pos_inf) {}
    };

    /*! tests if box is empty */
    template<typename T> FORCEINLINE bool IsEmpty(const BoundingBox<T>& box) { for (int i=0; i<T::N; i++) if (box.lower[i] > box.upper[i]) return true; return false; }

    /*! computes the center of the box */
    template<typename T> FORCEINLINE const T Center (const BoundingBox<T>& box) { return T(.5f)*(box.lower + box.upper); }
    template<typename T> FORCEINLINE const T Center2(const BoundingBox<T>& box) { return box.lower + box.upper; }

    /*! computes the size of the box */
    template<typename T> FORCEINLINE const T Size(const BoundingBox<T>& box) { return box.upper - box.lower; }

    /*! computes the volume of a bounding box */
    template<typename T> FORCEINLINE const T Volume( const BoundingBox<T>& b ) { return reduce_mul(size(b)); }

    /*! computes the surface area of a bounding box */
    template<typename T> FORCEINLINE const T     Area( const BoundingBox<Vector2<T> >& b ) { const Vector2<T> d = size(b); return d.x*d.y; }
    template<typename T> FORCEINLINE const T     Area( const BoundingBox<Vector3<T> >& b ) { const Vector3<T> d = size(b); return T(2)*(d.x*(d.y+d.z)+d.y*d.z); }
    template<typename T> FORCEINLINE const T HalfArea( const BoundingBox<Vector3<T> >& b ) { const Vector3<T> d = size(b); return d.x*(d.y+d.z)+d.y*d.z; }

    /*! merges bounding boxes and points */
    template<typename T> FORCEINLINE const BoundingBox<T> Merge( const BoundingBox<T>& a, const       T& b ) { return BoundingBox<T>(Min(a.lower, b    ), Max(a.upper, b    )); }
    template<typename T> FORCEINLINE const BoundingBox<T> Merge( const       T& a, const BoundingBox<T>& b ) { return BoundingBox<T>(Min(a    , b.lower), Max(a    , b.upper)); }
    template<typename T> FORCEINLINE const BoundingBox<T> Merge( const BoundingBox<T>& a, const BoundingBox<T>& b ) { return BoundingBox<T>(Min(a.lower, b.lower), Max(a.upper, b.upper)); }
    template<typename T> FORCEINLINE const BoundingBox<T> Merge( const BoundingBox<T>& a, const BoundingBox<T>& b, const BoundingBox<T>& c ) { return Merge(a,merge(b,c)); }
    template<typename T> FORCEINLINE const BoundingBox<T>& operator+=( BoundingBox<T>& a, const BoundingBox<T>& b ) { return a = Merge(a,b); }
    template<typename T> FORCEINLINE const BoundingBox<T>& operator+=( BoundingBox<T>& a, const       T& b ) { return a = Merge(a,b); }

    /*! Merges four boxes. */
    template<typename T> FORCEINLINE BoundingBox<T> Merge(const BoundingBox<T>& a, const BoundingBox<T>& b, const BoundingBox<T>& c, const BoundingBox<T>& d) 
    {
        return Merge(Merge(a,b),Merge(c,d));
    }

    /*! Merges eight boxes. */
    template<typename T> FORCEINLINE BoundingBox<T> Merge(const BoundingBox<T>& a, const BoundingBox<T>& b, const BoundingBox<T>& c, const BoundingBox<T>& d, const BoundingBox<T>& e, const BoundingBox<T>& f, const BoundingBox<T>& g, const BoundingBox<T>& h) 
    {
        return Merge(Merge(a,b,c,d),Merge(e,f,g,h));
    }

    /*! Comparison Operators */
    template<typename T> FORCEINLINE bool operator==( const BoundingBox<T>& a, const BoundingBox<T>& b ) { return a.lower == b.lower && a.upper == b.upper; }
    template<typename T> FORCEINLINE bool operator!=( const BoundingBox<T>& a, const BoundingBox<T>& b ) { return a.lower != b.lower || a.upper != b.upper; }

    /*! scaling */
    template<typename T> FORCEINLINE BoundingBox<T> operator *( const float& a, const BoundingBox<T>& b ) { return BoundingBox<T>(a*b.lower,a*b.upper); }

    /*! intersect bounding boxes */
    template<typename T> FORCEINLINE const BoundingBox<T> Intersect( const BoundingBox<T>& a, const BoundingBox<T>& b ) { return BoundingBox<T>(Max(a.lower, b.lower), Min(a.upper, b.upper)); }
    template<typename T> FORCEINLINE const BoundingBox<T> Intersect( const BoundingBox<T>& a, const BoundingBox<T>& b, const BoundingBox<T>& c ) { return Intersect(a,Intersect(b,c)); }

    /*! tests if bounding boxes (and points) are disjoint (empty intersection) */
    template<typename T> INLINE bool Disjoint( const BoundingBox<T>& a, const BoundingBox<T>& b )
    { const T d = Min(a.upper, b.upper) - Max(a.lower, b.lower); for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return true; return false; }
    template<typename T> INLINE bool Disjoint( const BoundingBox<T>& a, const  T& b )
    { const T d = Min(a.upper, b)       - Max(a.lower, b);       for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return true; return false; }
    template<typename T> INLINE bool Disjoint( const  T& a, const BoundingBox<T>& b )
    { const T d = Min(a, b.upper)       - Max(a, b.lower);       for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return true; return false; }

    /*! tests if bounding boxes (and points) are conjoint (non-empty intersection) */
    template<typename T> INLINE bool Conjoint( const BoundingBox<T>& a, const BoundingBox<T>& b )
    { const T d = Min(a.upper, b.upper) - Max(a.lower, b.lower); for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return false; return true; }
    template<typename T> INLINE bool Conjoint( const BoundingBox<T>& a, const  T& b )
    { const T d = Min(a.upper, b)       - Max(a.lower, b);       for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return false; return true; }
    template<typename T> INLINE bool Conjoint( const  T& a, const BoundingBox<T>& b )
    { const T d = Min(a, b.upper)       - Max(a, b.lower);       for ( size_t i = 0 ; i < T::N ; i++ ) if ( d[i] < typename T::Scalar(zero) ) return false; return true; }

    /*! subset relation */
    template<typename T> INLINE bool Subset( const BoundingBox<T>& a, const BoundingBox<T>& b )
    { 
        for ( size_t i = 0 ; i < T::N ; i++ ) if ( a.lower[i]*1.00001f < b.lower[i] ) return false; 
        for ( size_t i = 0 ; i < T::N ; i++ ) if ( a.upper[i] > b.upper[i]*1.00001f ) return false; 
        return true; 
    }

    /*! output operator */
    template<typename T> FORCEINLINE std::ostream& operator<<(std::ostream& cout, const BoundingBox<T>& box) 
    {
        return std::cout << "[" << box.lower << "; " << box.upper << "]";
    }
}
