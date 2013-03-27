//=============================================================================
// Vertex.h by Frank Luna (C) 2005 All Rights Reserved.
//
// This file contains all of our vertex structures.  We will add to it
// as needed throughout the book.
//=============================================================================
#pragma once

enum VertexType {VP, VC, VPN, VPNT};

//===============================================================
struct VertexPos
{
    VertexPos():pos(0.0f, 0.0f, 0.0f){}
    VertexPos(float x, float y, float z):pos(x,y,z){}
    VertexPos(const Base::Vector3& v):pos(v){}

    Base::Vector3 pos;
};

//===============================================================
struct VertexCol
{
    VertexCol():pos(0.0f, 0.0f, 0.0f),col(Base::Color4::Black){}
    VertexCol(float x, float y, float z, Base::Color4 c):pos(x,y,z), col(c){}
    VertexCol(const Base::Vector3& v, Base::Color4 c):pos(v),col(c){}

    Base::Vector3 pos;
    Base::Color4  col;
};

//===============================================================
struct VertexPN
{
    VertexPN()
        :pos(0.0f, 0.0f, 0.0f),
        normal(0.0f, 0.0f, 0.0f){}
    VertexPN(float x, float y, float z, 
        float nx, float ny, float nz):pos(x,y,z), normal(nx,ny,nz){}
    VertexPN(const Base::Vector3& v, const Base::Vector3& n)
        :pos(v),normal(n){}

    Base::Vector3 pos;
    Base::Vector3 normal;
};

//===============================================================
struct VertexPNT
{
    VertexPNT()
        :pos(0.0f, 0.0f, 0.0f),
        normal(0.0f, 0.0f, 0.0f),
        tex0(Base::Vector2::Zero){}
    VertexPNT(float x, float y, float z, 
        float nx, float ny, float nz,
        float u, float v):pos(x,y,z), normal(nx,ny,nz), tex0(u,v){}
    VertexPNT(const Base::Vector3& v, const Base::Vector3& n, const Base::Vector2& uv)
        :pos(v),normal(n), tex0(uv){}

    Base::Vector3 pos;
    Base::Vector3 normal;
    Base::Vector2 tex0;
};

//===============================================================
struct Particle
{
    Base::Vector3   initialPos;
    Base::Vector3   initialVelocity;
    float           initialSize; // In pixels.
    float           initialTime;
    float           lifeTime;
    float           mass;
    Base::Color4    initialColor;

};
