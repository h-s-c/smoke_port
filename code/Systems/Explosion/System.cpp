//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Explosion/Object.hpp"
#include "Systems/Explosion/Scene.hpp"
#include "Systems/Explosion/System.hpp"


///////////////////////////////////////////////////////////////////////////////
// ExplosionSystem - Constructor
ExplosionSystem::ExplosionSystem( void ) : ISystem()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~ExplosionSystem - Destructor
ExplosionSystem::~ExplosionSystem( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr ExplosionSystem::GetName( void )
{
    return System::Names::Explosion;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type ExplosionSystem::GetSystemType( void )
{
    return System::Types::Explosion;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error ExplosionSystem::Initialize( Properties::Array Properties )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void ExplosionSystem::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void ExplosionSystem::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene* ExplosionSystem::CreateScene( void )
{
    return new ExplosionScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destorys the given Scene, free all associated resources
Error ExplosionSystem::DestroyScene( ISystemScene* pSystemScene )
{
    ASSERT( pSystemScene != NULL );

    ExplosionScene* pScene = reinterpret_cast<ExplosionScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

