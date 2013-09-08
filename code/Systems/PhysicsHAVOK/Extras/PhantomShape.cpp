// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"

#include "Common\Base\hkBase.h"
#include "Physics2012\Dynamics\Entity\hkpRigidBody.h"
#include "Physics2012\Collide\Shape\hkpShape.h"
#include "Physics2012\Collide\Shape\Convex\Box\hkpBoxShape.h"
#include "Physics2012\Collide\Shape\Convex\Sphere\hkpSphereShape.h"
#include "Physics2012\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h"
#include "Physics2012\Collide\Shape\Deprecated\Mesh\hkpMeshShape.h"
#include "Physics2012\Collide\Shape\Compound\Tree\Mopp\hkpMoppCompilerInput.h"
#include "Physics2012\Collide\Shape\Compound\Tree\Mopp\hkpMoppUtility.h"
#include "Physics2012\Collide\Shape\Compound\Tree\Mopp\hkpMoppBvTreeShape.h"
#include "Physics2012\Collide\Shape\HeightField\hkpHeightFieldShape.h"
#include "Physics2012\Collide\Shape\Misc\Bv\hkpBvShape.h"
#include "Physics2012\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h"

// System
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"
#include "Systems/PhysicsHAVOK/Extras/PhantomShape.hpp"

#define PHAVOKSCENE         reinterpret_cast<HavokPhysicsScene*>(m_pSystemScene)


///////////////////////////////////////////////////////////////////////////////
// PhantomShape - Default constructor
PhantomShape::PhantomShape(
    HavokPhysicsScene* pSystemScene
    )
    : m_pSystemScene( pSystemScene )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~PhantomShape - Default destructor
PhantomShape::~PhantomShape(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// phantomEnterEvent - Callback that handle objects entering this phantom
void
PhantomShape::phantomEnterEvent(
    const hkpCollidable* pPhantomCollidable,
    const hkpCollidable* pOtherCollidable,
    const hkpCollisionInput& CollisionInput
    )
{
    //
    // Get the objects for the collidables.
    //
    hkpRigidBody* pOtherBody = reinterpret_cast<hkpRigidBody*>(pOtherCollidable->getOwner());
    hkpRigidBody* pPhantomBody = reinterpret_cast<hkpRigidBody*>(pPhantomCollidable->getOwner());

    HavokPhysicsObject* pSubject = reinterpret_cast<HavokPhysicsObject*>(pOtherBody->getUserData());
    HavokPhysicsObject* pObserver = reinterpret_cast<HavokPhysicsObject*>(pPhantomBody->getUserData());

    //
    // Track the movement within the body.
    //
    m_pSystemScene->AddTrackCollision( pSubject, pObserver );
}


///////////////////////////////////////////////////////////////////////////////
// phantomLeaveEvent - Callback that handle objects leaving this phantom
void
PhantomShape::phantomLeaveEvent(
    const hkpCollidable* pPhantomCollidable,
    const hkpCollidable* pOtherCollidable
    )
{
    //
    // Get the objects for the collidables.
    //
    hkpRigidBody* pOtherBody = reinterpret_cast<hkpRigidBody*>(pOtherCollidable->getOwner());
    hkpRigidBody* pPhantomBody = reinterpret_cast<hkpRigidBody*>(pPhantomCollidable->getOwner());

    HavokPhysicsObject* pSubject = reinterpret_cast<HavokPhysicsObject*>(pOtherBody->getUserData());
    HavokPhysicsObject* pObserver = reinterpret_cast<HavokPhysicsObject*>(pPhantomBody->getUserData());

    //
    // Removing tracking within the body.
    //
    m_pSystemScene->RemoveTrackCollision( pSubject, pObserver );
}
