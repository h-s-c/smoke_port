#include "Systems/ProceduralTrees/Trees/Tree.hpp"
#include "Systems/ProceduralTrees/Trees/Branch.hpp"

class OgreBranch : public Branch
{

  public:
    OgreBranch(std::int32_t level, Base::Vector3 basePosition);
    virtual Branch *CreateNextBranch(std::int32_t level, Base::Vector3 basePosition);
    virtual void fillBuffers();

  private:
    OgreBranch(){};
};
