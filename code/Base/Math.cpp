// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// Base
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
// Standard Library
#include <cmath>
#include <cstdint>
#include <cstdlib>


using namespace Base;


const float Angle::Pi = 3.1415926535897932384626433832795f;

const Vector2 Vector2::Zero = { 0.0f, 0.0f };
const Vector2 Vector2::One  = { 1.0f, 1.0f };

const Vector3 Vector3::Zero(0.0f);
const Vector3 Vector3::One(1.0f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);

const Vector4 Vector4::Zero = { 0.0f, 0.0f, 0.0f, 1.0f };
const Vector4 Vector4::One  = { 1.0f, 1.0f, 1.0f, 1.0f };

const Quaternion Quaternion::Zero = { 0.0f, 0.0f, 0.0f, 1.0f };

const Matrix4x4 Matrix4x4::Identity =
{
    {1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f}
};

const Color3 Color3::Black = { 0.0f, 0.0f, 0.0f };
const Color3 Color3::White = { 1.0f, 1.0f, 1.0f };

const Color4 Color4::Black = { 0.0f, 0.0f, 0.0f, 1.0f };
const Color4 Color4::White = { 1.0f, 1.0f, 1.0f, 1.0f };

// Wine, LGPL
const Vector3& Vector3::TransformCoord(Vector3& pout, const Vector3& pv, const Matrix4x4& pm)
{
    float norm;

    norm = pm.m[3] * pv.x + pm.m[7] * pv.y + pm.m[11] *pv.z + pm.m[15];

    if ( norm )
    {
     pout.x = (pm.m[0] * pv.x + pm.m[4] * pv.y + pm.m[8] * pv.z + pm.m[12]) / norm;
     pout.y = (pm.m[1] * pv.x + pm.m[5] * pv.y + pm.m[9] * pv.z + pm.m[13]) / norm;
     pout.z = (pm.m[2] * pv.x + pm.m[6] * pv.y + pm.m[10] * pv.z + pm.m[14]) / norm;
    }
    else
    {
     pout.x = 0.0f;
     pout.y = 0.0f;
     pout.z = 0.0f;
    }
    return pout;
}


const Quaternion&
Quaternion::Set(
    In Vector3& Axis,
    In float Angle
    )
{

    const float Sin = Angle::Sin( Angle / 2.0f );

    x = Axis.x * Sin;
    y = Axis.y * Sin;
    z = Axis.z * Sin;
    w = Angle::Cos( Angle / 2.0f );

    return *this;
}


const Quaternion&
Quaternion::Set(
    In Vector3& Angles
    )
{
    //
    // This function is by no means optimal since it it known that several of the coordinates
    //  are zero so can be ignored in the quaternion multiply.
    //
    Quaternion qx;
    Quaternion qy;
    Quaternion qz;

    qx.Set( Vector3::UnitX, Angles.x );
    qy.Set( Vector3::UnitY, Angles.y );
    qz.Set( Vector3::UnitZ, Angles.z );

    *this = qx * qy;
    *this *= qz;

    return *this;
}

//-----------------------------------------------------------------------------
// Apply a quaternion rotation to a vector.
//
void Quaternion::Rotate( Vector3& a )  
{
    Quaternion p, r, q, qInverse;
    
    // Convert vector to a quaternion
    p.x = a.x;
    p.y = a.y;
    p.z = a.z;
    p.w = 0.0f;
    
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    
    // This inverse only applicable if we are of unit length
    qInverse.x = -x;
    qInverse.y = -y;
    qInverse.z = -z;
    qInverse.w = w;
    
    // perform rotation
    r = q * p * qInverse;

    a.x = r.x;
    a.y = r.y;
    a.z = r.z;

    return;
}


Vector3
Matrix4x4::operator*(
    In Vector3& a
    ) const
{
    Vector3 r;

    r.x = m[ 0 ] * a.x + m[ 4 ] * a.y + m[  8 ] * a.z + m[ 12 ];
    r.y = m[ 1 ] * a.x + m[ 5 ] * a.y + m[  9 ] * a.z + m[ 13 ];
    r.z = m[ 2 ] * a.x + m[ 6 ] * a.y + m[ 10 ] * a.z + m[ 14 ];

    return r;
}


Vector4
Matrix4x4::operator*(
    In Vector4& a
    ) const
{
    Vector4 r;

    r.x = m[ 0 ] * a.x + m[ 4 ] * a.y + m[  8 ] * a.z + m[ 12 ] * a.w;
    r.y = m[ 1 ] * a.x + m[ 5 ] * a.y + m[  9 ] * a.z + m[ 13 ] * a.w;
    r.z = m[ 2 ] * a.x + m[ 6 ] * a.y + m[ 10 ] * a.z + m[ 14 ] * a.w;
    r.w = m[ 3 ] * a.x + m[ 7 ] * a.y + m[ 11 ] * a.z + m[ 15 ] * a.w;

    return r;
}


Matrix4x4
Matrix4x4::operator*(
    In Matrix4x4& a
    ) const
{
    Matrix4x4 r;

    std::uint32_t idx = 0;
    for ( std::uint32_t Row=0; Row < 16; Row+=4 )
    {
        for ( std::uint32_t Col=0; Col < 4; Col++ )
        {
            r.m[ idx ] = 0.0f;

            for ( std::uint32_t i=0; i < 4; i++ )
            {
                r.m[ idx ] += m[ Col + i*4 ] * a.m[ Row + i ];
            }

            idx++;
        }
    }

    return r;
}


const Matrix4x4&
Matrix4x4::operator*=(
    In Matrix4x4& a
    )
{
    *this = *this * a;
    return *this;
}


const Matrix4x4&
Matrix4x4::Transformation(
    const Vector3& Translation,
    const Quaternion& Orientation
    )
{
    SetTranslation( Translation );

    SetOrientation( Orientation );

    m[  3 ] = 0.0f;
    m[  7 ] = 0.0f;
    m[ 11 ] = 0.0f;
    m[ 15 ] = 1.0f;

    return *this;
}


void
Matrix4x4::GetOrientation(
    Quaternion& Orientation
    ) const
{
    const float T = 1.0f + m[ 0 ] + m[ 5 ] + m[ 10 ];

    if ( T > 0.00000001f )
    {
        const float S = std::sqrt( T ) * 2.0f;
        const float invS = 1.0f / S;

        Orientation.x = (m[ 6 ] - m[ 9 ]) * invS;
        Orientation.y = (m[ 8 ] - m[ 2 ]) * invS;
        Orientation.z = (m[ 1 ] - m[ 4 ]) * invS;
        Orientation.w = 0.25f * S;
    }
    else if ( m[ 0 ] > m[ 5 ] && m[ 0 ] > m[ 10 ] )
    {
        const float S = std::sqrt( 1.0f + m[ 0 ] - m[ 5 ] - m[ 10 ] ) * 2.0f;
        const float invS = 1.0f / S;

        Orientation.x = 0.25f * S;
        Orientation.y = (m[ 1 ] + m[ 4 ]) * invS;
        Orientation.z = (m[ 8 ] + m[ 2 ]) * invS;
        Orientation.w = (m[ 6 ] - m[ 9 ]) * invS;
    }
    else if ( m[ 5 ] > m[ 10 ] )
    {
        const float S  = std::sqrt( 1.0f + m[ 5 ] - m[ 0 ] - m[ 10 ] ) * 2.0f;
        const float invS = 1.0f / S;

        Orientation.x = (m[ 1 ] + m[ 4 ]) * invS;
        Orientation.y = 0.25f * S;
        Orientation.z = (m[ 6 ] + m[ 9 ]) * invS;
        Orientation.w = (m[ 8 ] - m[ 2 ]) * invS;
    }
    else
    {
        const float S  = std::sqrt( 1.0f + m[ 10 ] - m[ 0 ] - m[ 5 ] ) * 2.0f;
        const float invS = 1.0f / S;

        Orientation.x = (m[ 8 ] + m[ 2 ]) * invS;
        Orientation.y = (m[ 6 ] + m[ 9 ]) * invS;
        Orientation.z = 0.25f * S;
        Orientation.w = (m[ 1 ] - m[ 4 ]) * invS;
    }
}


const Matrix4x4&
Matrix4x4::SetOrientation(
    const Quaternion& Orientation
    )
{
    float xx = Orientation.x * Orientation.x;
    float xy = Orientation.x * Orientation.y;
    float xz = Orientation.x * Orientation.z;
    float xw = Orientation.x * Orientation.w;
    float yy = Orientation.y * Orientation.y;
    float yz = Orientation.y * Orientation.z;
    float yw = Orientation.y * Orientation.w;
    float zz = Orientation.z * Orientation.z;
    float zw = Orientation.z * Orientation.w;

    m[  0 ] = 1.0f - ((yy + zz) * 2.0f);
    m[  1 ] =        ((xy - zw) * 2.0f);
    m[  2 ] =        ((xz + yw) * 2.0f);
    m[  4 ] =        ((xy + zw) * 2.0f);
    m[  5 ] = 1.0f - ((xx + zz) * 2.0f);
    m[  6 ] =        ((yz - xw) * 2.0f);
    m[  8 ] =        ((xz - yw) * 2.0f);
    m[  9 ] =        ((yz + xw) * 2.0f);
    m[ 10 ] = 1.0f - ((xx + yy) * 2.0f);

    return *this;
}

const Matrix4x4&
Matrix4x4::Transpose(
    Out Matrix4x4& T
    )
{
    Matrix4x4 temp;
    temp = temp.Identity;
    for (int i = 0; i < 4; i++) {
    temp[i]      = T[i*4];
    temp[i + 4]  = T[i*4 + 1];
    temp[i + 8]  = T[i*4 + 2];
    temp[i + 12] = T[i*4 + 3];
    }

    T = temp;
    return T;
}                        
                         
const Matrix4x4&
Matrix4x4::Inverse(
    Out Matrix4x4& Inverse
    )
{

    Matrix4x4 A;

    A = Inverse;
    A = A.Transpose(A);

    
    
    float cofactor_00 = Cofactor9(A.m[5],A.m[6],A.m[7],A.m[9],A.m[10],A.m[11],A.m[13],A.m[14],A.m[15]);
    float cofactor_01 = -Cofactor9(A.m[4],A.m[6],A.m[7],A.m[8],A.m[10],A.m[11],A.m[12],A.m[14],A.m[15]);
    float cofactor_02 = Cofactor9(A.m[4],A.m[5],A.m[7],A.m[8],A.m[9],A.m[11],A.m[12],A.m[13],A.m[15]);
    float cofactor_03 = -Cofactor9(A.m[4],A.m[5],A.m[6],A.m[8],A.m[9],A.m[10],A.m[12],A.m[13],A.m[14]);
    float cofactor_10 = Cofactor9(A.m[1],A.m[2],A.m[3],A.m[9],A.m[10],A.m[11],A.m[13],A.m[14],A.m[15]);
    float cofactor_11 = Cofactor9(A.m[0],A.m[2],A.m[3],A.m[8],A.m[10],A.m[11],A.m[12],A.m[14],A.m[15]);
    float cofactor_12 = -Cofactor9(A.m[0],A.m[1],A.m[3],A.m[8],A.m[9],A.m[11],A.m[12],A.m[13],A.m[15]);
    float cofactor_13 = Cofactor9(A.m[0],A.m[1],A.m[2],A.m[8],A.m[9],A.m[10],A.m[12],A.m[13],A.m[14]);
    float cofactor_20 = Cofactor9(A.m[1],A.m[2],A.m[3],A.m[5],A.m[6],A.m[7],A.m[13],A.m[14],A.m[15]);
    float cofactor_21 = -Cofactor9(A.m[0],A.m[2],A.m[3],A.m[4],A.m[6],A.m[7],A.m[12],A.m[14],A.m[15]);
    float cofactor_22 = Cofactor9(A.m[0],A.m[1],A.m[3],A.m[4],A.m[5],A.m[7],A.m[12],A.m[13],A.m[15]);
    float cofactor_23 = -Cofactor9(A.m[0],A.m[1],A.m[2],A.m[4],A.m[5],A.m[6],A.m[12],A.m[13],A.m[14]);
    float cofactor_30 = -Cofactor9(A.m[1],A.m[2],A.m[3],A.m[5],A.m[6],A.m[7],A.m[9],A.m[10],A.m[11]);
    float cofactor_31 = Cofactor9(A.m[0],A.m[2],A.m[3],A.m[4],A.m[6],A.m[7],A.m[8],A.m[10],A.m[11]);
    float cofactor_32 = -Cofactor9(A.m[0],A.m[1],A.m[3],A.m[4],A.m[5],A.m[7],A.m[8],A.m[9],A.m[11]);
    float cofactor_33 = Cofactor9(A.m[0],A.m[1],A.m[2],A.m[4],A.m[5],A.m[6],A.m[8],A.m[9],A.m[10]);

    float cofactorGiven_00 = Cofactor9(Inverse.m[5],Inverse.m[6],Inverse.m[7],Inverse.m[9],Inverse.m[10],Inverse.m[11],Inverse.m[13],Inverse.m[14],Inverse.m[15]);
    float cofactorGiven_01 = Cofactor9(Inverse.m[4],Inverse.m[6],Inverse.m[7],Inverse.m[8],Inverse.m[10],Inverse.m[11],Inverse.m[12],Inverse.m[14],Inverse.m[15]);
    float cofactorGiven_02 = Cofactor9(Inverse.m[4],Inverse.m[5],Inverse.m[7],Inverse.m[8],Inverse.m[9],Inverse.m[11],Inverse.m[12],Inverse.m[13],Inverse.m[15]);
    float cofactorGiven_03 = Cofactor9(Inverse.m[4],Inverse.m[5],Inverse.m[6],Inverse.m[8],Inverse.m[9],Inverse.m[10],Inverse.m[12],Inverse.m[13],Inverse.m[14]);
    float detA = Inverse.m[0] * cofactorGiven_00 - Inverse.m[1]*cofactorGiven_01 + Inverse.m[2]*cofactorGiven_02 - Inverse.m[3]*cofactorGiven_03;
    float IdetA = 1.0f/detA;

    Inverse.m[0]   = IdetA * cofactor_00;
    Inverse.m[1]   = IdetA * cofactor_01;
    Inverse.m[2]   = IdetA * cofactor_02;
    Inverse.m[3]   = IdetA * cofactor_03;
    Inverse.m[4]   = IdetA * cofactor_10;
    Inverse.m[5]   = IdetA * cofactor_11;
    Inverse.m[6]   = IdetA * cofactor_12;
    Inverse.m[7]   = IdetA * cofactor_13;
    Inverse.m[8]   = IdetA * cofactor_20;
    Inverse.m[9]   = IdetA * cofactor_21;
    Inverse.m[10]  = IdetA * cofactor_22;
    Inverse.m[11]  = IdetA * cofactor_23;
    Inverse.m[12]  = IdetA * cofactor_30;
    Inverse.m[13]  = IdetA * cofactor_31;
    Inverse.m[14]  = IdetA * cofactor_32;
    Inverse.m[15]  = IdetA * cofactor_33;

    return Inverse;
}

float Matrix4x4::Cofactor4(float a, float b, float c, float d)
{
    return (a*d)-(b*c);
}

float Matrix4x4::Cofactor9(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
    return (a*Cofactor4(e,f,h,i)) + (b*Cofactor4(d,f,g,i)) + (c*Cofactor4(d,e,g,h));

}

// Wine, LGPL
const Matrix4x4& Matrix4x4::MatrixRotationAxis(Matrix4x4& pout, const Vector3& pv, float angle)
{
    Vector3 v = pv;

    v = v.Normalize();
    pout = pout.Identity;
    pout.m[0] = (1.0f - std::cos(angle)) * v.x * v.x + std::cos(angle);
    pout.m[1] = (1.0f - std::cos(angle)) * v.x * v.y - std::sin(angle) * v.z;
    pout.m[2] = (1.0f - std::cos(angle)) * v.x * v.z + std::sin(angle) * v.y;
    pout.m[4] = (1.0f - std::cos(angle)) * v.y * v.x + std::sin(angle) * v.z;
    pout.m[5] = (1.0f - std::cos(angle)) * v.y * v.y + std::cos(angle);
    pout.m[6] = (1.0f - std::cos(angle)) * v.y * v.z - std::sin(angle) * v.x;
    pout.m[8] = (1.0f - std::cos(angle)) * v.z * v.x - std::sin(angle) * v.y;
    pout.m[9] = (1.0f - std::cos(angle)) * v.z * v.y + std::sin(angle) * v.x;
    pout.m[10] = (1.0f - std::cos(angle)) * v.z * v.z + std::cos(angle);
    return pout;
} 

const Matrix4x4& Matrix4x4::MatrixRotationYawPitchRoll(Matrix4x4& pout, float roll, float pitch, float yaw )
{
    pout.m[0] = ( std::cos(roll) * std::cos(yaw) ) + ( std::sin(roll) * std::sin(pitch) * std::sin(yaw) );
    pout.m[1] = ( std::sin(roll) * std::cos(pitch) );
    pout.m[2] = ( std::cos(roll) * -std::sin(yaw) ) + ( std::sin(roll) * std::sin(pitch) * std::cos(yaw) );
    pout.m[4] = (-std::sin(roll) * std::cos(yaw) ) + ( std::cos(roll) * std::sin(pitch) * std::sin(yaw) );
    pout.m[5] = ( std::cos(roll) * std::cos(pitch) );
    pout.m[6] = ( std::sin(roll) * std::sin(yaw) ) + ( std::cos(roll) * std::sin(pitch) * std::cos(yaw) );
    pout.m[8] = ( std::cos(pitch) * std::sin(yaw) );
    pout.m[9] = -std::sin(pitch);
    pout.m[10] = ( std::cos(pitch) * std::cos(yaw) );
    pout.m[15] = 1;
    return pout;
}


float Random::GetRandomFloat(float a, float b)
{
    if( a >= b ) // bad input
        return a;

    // Get random float in [0, 1] interval.
    float f = (rand()%10001) * 0.0001f;

    return (f*(b-a))+a;
}
