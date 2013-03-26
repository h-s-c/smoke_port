//Tree Class
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
#pragma once

class Tree;
struct treeNode;
#include "Systems/ProceduralTrees/Trees/Branch.hpp"
#include "Systems/ProceduralTrees/Trees/Observer.hpp"
#include <vector>


struct AABB;
class Canopy;
class Grammar;
class Tree;

struct treeNode {
    BranchBase *pbranch;
    treeNode *pPrevNode;
    treeNode *pNextNodes;
    int nextNodeSize;
    Tree *tree;
};

class Tree {
public:
    Tree(std::string grammarName, Base::Vector3 position);
    ~Tree();
    virtual Branch* createTrunk(int level, Base::Vector3 basePosition);
    void deleteNode(treeNode *ctn);
    void BoundingBox(treeNode *ctn, bool Canopy=false);
    void countVertices(treeNode *ctn);
    void countBranches(treeNode *ctn);
    void getBranch(treeNode *ctn, int branch);
    virtual void growTree(bool justCount=false);
    virtual void growCanopy(treeNode *ctn);
    //virtual void growCanopySegment(treeNode *rootOfTree, branchBase *pCanopyBranch);
    // pruneTree will be a function to split up the tree or remove branches.  
    // It is strictly a placeholder for future development
    void pruneTree(Base::Vector3 *pVertexBuffer);
    void sprout();
    void fillBuffers(Canopy *cnpy);
    void fillBuffers(BranchBase *branch);
    void fillBranches(treeNode *ctn);
    void fillCanopies(treeNode *ctn);
    void BranchToList(treeNode *ctn, std::vector<BranchBase*> *treeNodeList);
    void CanopyToList(treeNode *ctn, std::vector<Canopy*> *treeNodeCanopyList);

    observer *theOverseer;
    AABB m_boundingBox;
    AABB *m_pBoundingboxes;
    Grammar m_grammar;
    int m_levelCount;
    Base::Vector3 m_position;
    long m_StartBranchIndexBuffer;
    long m_EndBranchIndexBuffer;
    long m_StartBranchVertexBuffer;
    long m_EndBranchVertexBuffer;
    long m_StartCanopyIndexBuffer;
    long m_EndCanopyIndexBuffer;
    long m_StartCanopyVertexBuffer;
    long m_EndCanopyVertexBuffer;
    long m_StartAttribBuffer;
    long m_EndAttribBuffer;
    treeNode *m_nodeTree; // node tree of branch base points.  doesn't include segment info.
    bool m_OnFire;
    int m_Life;
    int m_FaceCount;
    int m_BranchCount;
    int m_CanopyCount;
    int m_CanopyCountTest;
    static bool counting;
    // this is a function used strictly for Release mode debugging. 
    // you can pass a variable into this function and the compiler won't optimize it out.  
    virtual void KeepVariableAlive(void * a);

protected:
    void *                                  m_KAV;

    void bounds(BranchBase *branch);
    Grammar * createGrammar(std::string grammarName);
    void bounds(Canopy *Canopy);
    Tree(){};

};
