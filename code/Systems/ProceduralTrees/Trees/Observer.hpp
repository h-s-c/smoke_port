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


// When dealing with linked lists or tree structures it is often cumbersome to deal
// with the data used in the recursive functions.  trying to keep track of where you
// are can get confusing and difficult.  The choices for the programmar are to either pass a lot 
// of data in the function creating a lot of stack bloat or create a lot of global variables.
// I'm not personally fond of either solution so the observer class was created as a singleton
// container that will basically manage those variables for you.  A managed global container
// (not to be confused with the Observer Pattern).  It oversees the variables needed and provides
// a way for the application using the library to pass in the data without having extern in or out a bunch
// of variables.  You can call the instance at any point and know you have the variables in there present state.
//
#pragma once

#include "Systems/Common/AABB.hpp"
#include "Systems/ProceduralTrees/Trees/Vertex.hpp"
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <random>


class observer;

struct RenderStructure {
    VertexType type;    
    VertexPos *vpos;  //vertex buffer pointer
    VertexPNT *vpnt;  //vertex buffer pointer
    std::uint16_t *ptrIBData;  //index buffer pointer
    std::uint32_t CurrentIndex;
    std::uint32_t CurrentVIndex; 
    std::uint16_t BranchCount;
    bool ReverseWindingOrder;
    AABB aabb;
};

class Tree;
class BranchBase;
class observer 
{
    // Singleton
    static std::shared_ptr<observer> instance_;
    static std::once_flag                   only_one;
     
    observer(const observer& rs) {
        instance_  = rs.instance_;
    }
 
    observer& operator = (const observer& rs) 
    {
        if (this != &rs) {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    observer() {}
    
public:

    static observer& Instance() 
    {
        std::call_once( observer::only_one, [] () 
        { 
            observer::instance_.reset( 
                new observer()); 
        });
 
        return *observer::instance_;
    }
    
    ~observer() {}

    static RenderStructure *DXRS;
    void BoundingBox(Base::Vector3 vertex);

    int addVertex(VertexType type, Base::Vector3 vertex, Base::Vector3 normal, Base::Vector3 theTexture);
    void addIndexes(std::uint16_t a, std::uint16_t b, std::uint16_t c);
    float randf(float f);//defaults to number between 0(inclusive)-1(exclusive) 
    float randf(float min, float max);
    std::int32_t randi(std::int32_t i);
    std::int32_t randi(std::int32_t min, std::int32_t max);
    Tree *m_tree;
    BranchBase *m_SoughtAfterBranch;
    bool m_branchFound;
    
  private:
    std::default_random_engine m_randomEngine;
    
};
