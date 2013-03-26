//OgreTree Class
//Writen by Hugh Smith April 2007
// Part of Tree Grammar Structure for creating procedural trees 
// Created for Nehalem Demo to be compatable with OGRE Rendering methodologies

#include "Systems/ProceduralTrees/Trees/Tree.hpp"

class OgreTree : public Tree {
public: 
    OgreTree(std::string grammarName, Base::Vector3 position){}
    ~OgreTree(){}
    virtual Branch* createTrunk(int level, Base::Vector3 basePosition);
private:
    OgreTree(){};
};
