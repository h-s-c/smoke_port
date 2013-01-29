//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Explosion/Object.hpp"
#include "Systems/Explosion/Scene.hpp"
#include "Systems/Explosion/Task.hpp"


///////////////////////////////////////////////////////////////////////////////
// ExplosionTask - Constructor
ExplosionTask::ExplosionTask( ExplosionScene* pScene ) : ISystemTask( pScene ) , m_pScene( pScene )
{
    ASSERT( m_pScene != NULL );
}


///////////////////////////////////////////////////////////////////////////////
// ~ExplosionTask - Destructor
ExplosionTask::~ExplosionTask( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Task
System::Type ExplosionTask::GetSystemType( void )
{
    return System::Types::Explosion;
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update the Explosion system (this is where all the work gets done)
void ExplosionTask::Update( f32 DeltaTime )
{
    // Make sure DeltaTime isn't too large
    if ( DeltaTime > 0.04f )
    {
        DeltaTime = 0.04f;
    }

    // Update - process Explosions
    m_pScene->Update( DeltaTime );
}

