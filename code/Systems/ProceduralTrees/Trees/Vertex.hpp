//=============================================================================
// Vertex.h by Frank Luna (C) 2005 All Rights Reserved.
//
// This file contains all of our vertex structures.  We will add to it
// as needed throughout the book.
//=============================================================================
#include <d3dx9.h>

#ifndef VERTEX_H
#define VERTEX_H



// Call in constructor and destructor, respectively, of derived application class.
void InitAllVertexDeclarations();
//void DestroyAllVertexDeclarations();
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
    VertexCol():pos(0.0f, 0.0f, 0.0f),col(0x00000000){}
    VertexCol(float x, float y, float z, D3DCOLOR c):pos(x,y,z), col(c){}
    VertexCol(const D3DXVECTOR3& v, D3DCOLOR c):pos(v),col(c){}

    D3DXVECTOR3 pos;
    D3DCOLOR    col;
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
        tex0(0.0f, 0.0f){}
    VertexPNT(float x, float y, float z, 
        float nx, float ny, float nz,
        float u, float v):pos(x,y,z), normal(nx,ny,nz), tex0(u,v){}
    VertexPNT(const Base::Vector3& v, const Base::Vector3& n, const D3DXVECTOR2& uv)
        :pos(v),normal(n), tex0(uv){}

    Base::Vector3 pos;
    Base::Vector3 normal;
    D3DXVECTOR2 tex0;
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
    D3DCOLOR        initialColor;

};

#endif // VERTEX_H
