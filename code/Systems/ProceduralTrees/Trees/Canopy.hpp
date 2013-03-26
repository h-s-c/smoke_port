#pragma once

#include "Base/Math.hpp"
#include "Systems/Common/AABB.hpp"
#include "Systems/ProceduralTrees/Trees/Observer.hpp"
#include "Systems/ProceduralTrees/Trees/SpeciesGrammar.hpp"
#include "Systems/ProceduralTrees/Trees/Tree.hpp"

#include <cstdint>
#include <vector>


class B3
{
public:
    B3(){};
    B3(Base::Vector3 i,Base::Vector3 j, Base::Vector3 k)
    { 
        iAxis = i;
        jAxis = j;
        kAxis = k;
    };
    void normalize()
    {
        iAxis.Normalize();
        jAxis.Normalize();
        kAxis.Normalize();
    };
    Base::Vector3 iAxis;
    Base::Vector3 jAxis;
    Base::Vector3 kAxis;

};


enum CanopyType {HEX_CANOPY=0x01, PATCH_CANOPY=0x10, PROCEDURAL_CANOPY=0x100, TIP_CANOPY=0x1000};

class Canopy {
public: 
    //canopy(){};
    Canopy(int baseWidth = 9,int baseHeight =9,float WidthStep = 1.0f, float HeightStep = 1.25f)
    {
        m_width      = baseWidth ; 
        m_height     = baseHeight; 
        m_widthStep  = WidthStep ; 
        m_heightStep = HeightStep; 
    };
    void heading(Base::Vector3 Heading);
    Base::Vector3 AxisHeading; // unlike the branch heading this is the normal from the plane of the tipPointList
    B3 Basis; 
    CanopyType type; 
    std::vector<Base::Vector3 *> m_Canopy;
    int   m_width     ;
    int   m_height    ;
    float m_widthStep ;
    float m_heightStep;
    int m_startVertex;
    int m_startIndex; 
    int m_vertexCount;
    int m_indexCount; 
    int m_attribute;
    AABB m_AABB;
    bool m_burning;
    observer *theOverseer;
    int m_nodeLevel; //level of grammar or present depth of tree growth.
    LevelDetail *m_pSpeciesLevelGrammar; //just the facts needed for this branch. no sense dragging the whole grammar along
    void growCanopySegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, Grammar *grammar);
    void growPatchSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, LevelDetail *grammar, Base::Vector3 startHeading);
    void growHexSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,AABB TreeBoundingBox, LevelDetail *grammar, Base::Vector3 startHeading);

private:
    AABB setAABB();
};
