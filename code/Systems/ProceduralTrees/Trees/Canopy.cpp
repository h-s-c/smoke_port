//canopy Class
//Written by Hugh Smith November 2007
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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/ProceduralTrees/Trees/Tree.hpp"
#include "Systems/ProceduralTrees/Trees/Canopy.hpp"

#include <limits>

void Canopy::heading(Base::Vector3 Heading)
{
    AxisHeading = Heading;
}

AABB Canopy::setAABB(){
    m_AABB.xMin = std::numeric_limits<float>::max();
    m_AABB.yMin = std::numeric_limits<float>::max();
    m_AABB.zMin = std::numeric_limits<float>::max();
    m_AABB.xMax = -std::numeric_limits<float>::max();
    m_AABB.yMax = -std::numeric_limits<float>::max();
    m_AABB.zMax = -std::numeric_limits<float>::max();
    m_AABB.max = Base::Vector3(m_AABB.xMax,
                  m_AABB.yMax,
                  m_AABB.zMax);
    m_AABB.min = Base::Vector3(m_AABB.xMin,
                  m_AABB.yMin,
                  m_AABB.zMin);
    std::vector<Base::Vector3*>::iterator it,itend;
    it = m_Canopy.begin();
    itend = m_Canopy.end();
    for(it;it!=itend;it++){
            Base::Vector3 vertex = *(*it);
            if (m_AABB.xMin > vertex.x) m_AABB.xMin = vertex.x;
            if (m_AABB.xMax < vertex.x) m_AABB.xMax = vertex.x;
            if (m_AABB.yMin > vertex.y) m_AABB.yMin = vertex.y;
            if (m_AABB.yMax < vertex.y) m_AABB.yMax = vertex.y;
            if (m_AABB.zMin > vertex.z) m_AABB.zMin = vertex.z;
            if (m_AABB.zMax < vertex.z) m_AABB.zMax = vertex.z;
            m_AABB.min = Base::Vector3(m_AABB.xMin,m_AABB.yMin,m_AABB.zMin);
            m_AABB.max = Base::Vector3(m_AABB.xMax,m_AABB.yMax,m_AABB.zMax);
    }

    return m_AABB;

}

// The patch segment is an array of points that will be used as a canopy patch at the end of a branch or where
// a canopy is defined.  To create the patch we need to know what the drop angle will be. This is calculated based on the established 
// grammar drop angle along with the fulcrum calculation for the tree.  The patch will not be flat but will be perturbed from the plane in
// a random way based on a given weight factor for perturbation.  
void Canopy::growPatchSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, LevelDetail *grammar, Base::Vector3 startHeading)
{
    //int cwidth =9;
    //int cheight =9;
    //float m_widthStep =1;
    //float m_heightStep =1.25;
    rootOfTree->tree->m_CanopyCount++;
    Base::Vector3 root(rootOfTree->pbranch->segments[0].m_tipPointList[0]);
    Base::Vector3 CanopySegmentRoot(pCanopyBranch->tipPoint);// canopySegmentRoot
    Base::Vector3 CanopyTop(root);
    CanopyTop.y = root.y + TreeBoundingBox.yMax;
    Base::Vector3 CanopyFulcrum(CanopyTop);
    CanopyFulcrum.y = CanopyFulcrum.y * 0.57f; // 4/7
    if(pCanopyBranch->tipPoint.y < CanopyFulcrum.y){
        CanopyFulcrum.y = pCanopyBranch->tipPoint.y * 0.714f;// 5/7
    }
    Base::Vector3 CanopyHeading = CanopySegmentRoot-CanopyFulcrum;
    CanopyHeading.Normalize();
    Base::Vector3 CanopyArbitrary(CanopySegmentRoot-root);
    CanopyArbitrary.Normalize();
    Base::Vector3 Left = CanopyArbitrary.Cross(CanopyHeading);
    Base::Vector3 Right = CanopyHeading.Cross(CanopyArbitrary);
    Left.Normalize();
    Right.Normalize();
    Base::Vector3 Down = Left.Cross(CanopyHeading);
    Base::Vector3 Up = CanopyHeading.Cross(Left);
    float perturbFactor = grammar->perturbFactor;
    Base::Vector3 perturb = CanopyHeading;
    Down.Normalize();
    Up.Normalize();
    Basis.iAxis = Right * (((float)m_width)/2) * m_widthStep;
    Basis.jAxis = CanopyHeading;
    Basis.kAxis = Down * (((float)m_height)/2) * m_heightStep;
    int shift = -((int)floor(m_width/2.0f));
    float nudge =0.0f;
    for(int j=0;j<m_height;j++){
        for(int i=0;i<m_width;i++){
            nudge = observer::Instance().randf(-(perturbFactor),(perturbFactor));
            perturb = perturb * nudge;
            Base::Vector3 tcb = pCanopyBranch->tipPoint + (Left * (i+shift * m_widthStep)) + (Down * j * this->m_heightStep);
            Base::Vector3 *cb;
            if(!(j==0&&i==(m_width/2))){
                tcb = tcb + perturb;
                cb = new Base::Vector3(tcb);
            }else{
                cb = new Base::Vector3(tcb);
            }
            m_Canopy.push_back(cb);
            perturb = CanopyHeading;
        }
    }
    m_startIndex = observer::Instance().DXRS->CurrentIndex;
    m_startVertex = observer::Instance().DXRS->CurrentVIndex;
    m_AABB = setAABB();
    observer::Instance().DXRS->CurrentIndex = observer::Instance().DXRS->CurrentIndex + ((m_height-1)*(m_width-1)*12); // 6 per quad per side = 12
    observer::Instance().DXRS->CurrentVIndex = observer::Instance().DXRS->CurrentVIndex + ( m_height*m_width*2); // two sides = 2
    m_burning = false;
   
}
void Canopy::growHexSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, LevelDetail *grammar, Base::Vector3 startHeading)
{
    rootOfTree->tree->m_CanopyCount++;
    Base::Vector3 root(rootOfTree->pbranch->segments[0].m_tipPointList[0]);
    Base::Vector3 CanopySegmentRoot(pCanopyBranch->tipPoint);// canopySegmentRoot
    Base::Vector3 CanopyTop(root);
    CanopyTop.y = root.y + TreeBoundingBox.yMax;
    Base::Vector3 CanopyFulcrum(CanopyTop);
    CanopyFulcrum.y = CanopyFulcrum.y * 0.57f; // 4/7
    if(pCanopyBranch->tipPoint.y < CanopyFulcrum.y){
        CanopyFulcrum.y = pCanopyBranch->tipPoint.y * 0.714f;// 5/7
    }
    Base::Vector3 CanopyHeading = CanopySegmentRoot-CanopyFulcrum;
    CanopyHeading.Normalize();
    Base::Vector3 CanopyArbitrary(CanopySegmentRoot-root);
    CanopyArbitrary.Normalize();
    Base::Vector3 Left = CanopyArbitrary.Cross(CanopyHeading);
    Base::Vector3 Right = CanopyHeading.Cross(CanopyArbitrary);
    Left.Normalize();
    Right.Normalize();
    Base::Vector3 Down = Left.Cross(CanopyHeading);
    Base::Vector3 Up = CanopyHeading.Cross(Left);
    Base::Vector3 perturb = CanopyHeading;
    Down.Normalize();
    Up.Normalize();
    int shift = -((int)floor(m_width/2.0f));
    float nudge =0.0f;

    growPatchSegment(rootOfTree, pCanopyBranch,TreeBoundingBox, grammar,startHeading);
}

void Canopy::growCanopySegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, Grammar *grammar)
{

    bool grammarFound = false;
    int level =0;
    while(!grammarFound){
        if(grammar->m_pLevels[level].cnpyType != canopyType::NULL_CANOPY){
            grammarFound = true;
        }else{
            level++;
        }
    }
    canopyType ct = grammar->m_pLevels[level].cnpyType;
    switch(ct){
        case canopyType::HEX:
            {
                pCanopyBranch->canopies->type = CanopyType::HEX_CANOPY;
                m_nodeLevel = level;
                m_pSpeciesLevelGrammar = &grammar->m_pLevels[m_nodeLevel];
                growHexSegment(rootOfTree, pCanopyBranch,TreeBoundingBox, m_pSpeciesLevelGrammar,pCanopyBranch->heading);           
                break;
            }
        case canopyType::PATCH:
            {
                pCanopyBranch->canopies->type = CanopyType::HEX_CANOPY;
                m_nodeLevel = level;
                m_pSpeciesLevelGrammar = &grammar->m_pLevels[m_nodeLevel];
                growPatchSegment(rootOfTree, pCanopyBranch,TreeBoundingBox, m_pSpeciesLevelGrammar,pCanopyBranch->heading);
                break;
            }
    }
}
