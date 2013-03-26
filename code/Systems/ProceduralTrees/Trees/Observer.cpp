//Observer Class
//Written by Hugh Smith April 2007
// Part of Tree Grammar Structure for creating procedural trees

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

#include "Systems/ProceduralTrees/Trees/Observer.hpp"

std::once_flag             
observer::only_one;
std::shared_ptr<observer> 
observer::instance_ = nullptr;

RenderStructure *observer::DXRS =0; 

void observer::BoundingBox(Base::Vector3 vertex){
    if (DXRS->aabb.xMin > vertex.x) DXRS->aabb.xMin = vertex.x;
    if (DXRS->aabb.xMax < vertex.x) DXRS->aabb.xMax = vertex.x;
    if (DXRS->aabb.yMin > vertex.y) DXRS->aabb.yMin = vertex.y;
    if (DXRS->aabb.yMax < vertex.y) DXRS->aabb.yMax = vertex.y;
    if (DXRS->aabb.zMin > vertex.z) DXRS->aabb.zMin = vertex.z;
    if (DXRS->aabb.zMax < vertex.z) DXRS->aabb.zMax = vertex.z;
    DXRS->aabb.min = Base::Vector3(DXRS->aabb.xMin,DXRS->aabb.yMin,DXRS->aabb.zMin);
    DXRS->aabb.max = Base::Vector3(DXRS->aabb.xMax,DXRS->aabb.yMax,DXRS->aabb.zMax);
}
int observer::addVertex(VertexType type, Base::Vector3 vertex, Base::Vector3 normal = Base::Vector3(0.0,1.0,0.0), Base::Vector3 theTexture = Base::Vector3(0.5, 0.5, 0.0))
{
    BoundingBox(vertex);
    if (type == VertexType::VPNT && DXRS->vpnt) {
        DXRS->vpnt[DXRS->CurrentVIndex++] = VertexPNT(vertex.x, vertex.y, vertex.z,normal.x, normal.y, normal.z, theTexture.x, theTexture.y);
    }
    if (type == VertexType::VP && DXRS->vpos) {
        DXRS->vpos[DXRS->CurrentVIndex++] = VertexPos(vertex.x, vertex.y, vertex.z); 
    }
   return 1;
}
void observer::addIndexes(std::uint16_t a, std::uint16_t b, std::uint16_t c)
{
    if (DXRS->ptrIBData) {
        DXRS->ptrIBData[DXRS->CurrentIndex++]=a;
        DXRS->ptrIBData[DXRS->CurrentIndex++]=b;
        DXRS->ptrIBData[DXRS->CurrentIndex++]=c;
    }
}



float observer::randf(float max = 1.0f){
    std::uniform_real_distribution<float> distribution(std::numeric_limits<float>::min(),max);
    return distribution(m_randomEngine);
}
float observer::randf(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min,max);
    return distribution(m_randomEngine);
}
std::int32_t observer::randi(std::int32_t max=101)
{
    std::uniform_int_distribution<std::int32_t> distribution(std::numeric_limits<std::int32_t>::min(),max);
    return distribution(m_randomEngine);
}
std::int32_t observer::randi(std::int32_t min, std::int32_t max)
{
    std::uniform_int_distribution<std::int32_t> distribution(min,max);
    return distribution(m_randomEngine);
}
