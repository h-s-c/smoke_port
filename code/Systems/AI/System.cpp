//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Object.hpp"
#include "Systems/AI/Scene.hpp"
#include "Systems/AI/System.hpp"


///////////////////////////////////////////////////////////////////////////////
// AISystem - Constructor
AISystem::AISystem( void ) : ISystem()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~AISystem - Destructor
AISystem::~AISystem( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr AISystem::GetName( void )
{
    return System::Names::AI;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type AISystem::GetSystemType( void )
{
    return System::Types::AI;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error AISystem::Initialize( Properties::Array Properties )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void AISystem::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void AISystem::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene* AISystem::CreateScene( void )
{
    return new AIScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destorys the given Scene, free all associated resources
Error AISystem::DestroyScene( ISystemScene* pSystemScene )
{
    ASSERT( pSystemScene != NULL );

    AIScene* pScene = reinterpret_cast<AIScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

