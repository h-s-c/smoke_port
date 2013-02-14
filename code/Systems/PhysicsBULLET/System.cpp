// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
#include "Systems/PhysicsBULLET/ObjectCollision.hpp"
#include "Systems/PhysicsBULLET/Scene.hpp"
#include "Systems/PhysicsBULLET/System.hpp"


extern ManagerInterfaces    g_Managers;


///////////////////////////////////////////////////////////////////////////////
// BulletPhysicsSystem - Constructor
BulletPhysicsSystem::BulletPhysicsSystem(
    void
    )
    : ISystem()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~BulletPhysicsSystem - Destructor
BulletPhysicsSystem::~BulletPhysicsSystem(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr
BulletPhysicsSystem::GetName(
    void
    )
{
    return System::Names::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type
BulletPhysicsSystem::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error
BulletPhysicsSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void
BulletPhysicsSystem::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void
BulletPhysicsSystem::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}

///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene*
BulletPhysicsSystem::CreateScene(
    void
    )
{
    return new BulletPhysicsScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destroys the given Scene, free all associated resources
Error
BulletPhysicsSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    BulletPhysicsScene* pScene = reinterpret_cast<BulletPhysicsScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}
