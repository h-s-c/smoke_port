#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/ProceduralTrees/Trees/OgreTree.hpp"
#include "Systems/ProceduralTrees/Trees/OgreBranch.hpp"


Branch* OgreTree::createTrunk(int level, Base::Vector3 basePosition)
{
    return new OgreBranch(level,basePosition);
}


