#pragma once

#include "Base/Math.hpp"
#include <vector>
#include "System/Common/AABB.hpp"
#include "System/ProceduralTrees/Trees/Observer.hpp"
#include "System/ProceduralTrees/Trees/SpeciesGrammar.hpp"
struct treeNode;
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
    void heading(V3 Heading);
    V3 AxisHeading; // unlike the branch heading this is the normal from the plane of the tipPointList
    B3 Basis; 
    CanopyType type; 
    std::vector<V3 *> m_Canopy;
    int   m_width     ;
    int   m_height    ;
    float m_widthStep ;
    float m_heightStep;
    int m_startVertex;
    int m_startIndex; 
    int m_vertexCount;
    int m_indexCount; 
    int m_attribute;
    aabb m_AABB;
    bool m_burning;
    observer *theOverseer;
    int m_nodeLevel; //level of grammar or present depth of tree growth.
    LevelDetail *m_pSpeciesLevelGrammar; //just the facts needed for this branch. no sense dragging the whole grammar along
    void growCanopySegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,aabb TreeBoundingBox, Grammar *grammar);
    void growPatchSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,aabb TreeBoundingBox, LevelDetail *grammar, V3 startHeading);
    void growHexSegment(treeNode *rootOfTree, BranchBase *pCanopyBranch,aabb TreeBoundingBox, LevelDetail *grammar, V3 startHeading);

private:
    aabb setAABB();
};
