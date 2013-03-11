//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Ai/Object.hpp"
#include "Systems/Ai/Scene.hpp"
#include "Systems/Ai/Task.hpp"


///////////////////////////////////////////////////////////////////////////////
// AITask - Constructor
AITask::AITask( AIScene* pScene ) : ISystemTask( pScene ) , m_pScene( pScene )
{
    ASSERT( m_pScene != NULL );
}


///////////////////////////////////////////////////////////////////////////////
// ~AITask - Destructor
AITask::~AITask( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Task
System::Type AITask::GetSystemType( void )
{
    return System::Types::AI;
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update the AI system (this is were all the work gets done)
void AITask::Update( f32 DeltaTime )
{
    // Make sure DeltaTime isn't too large
    if ( DeltaTime > 0.04f )
    {
        DeltaTime = 0.04f;
    }

    // Update - process AIs
    m_pScene->Update( DeltaTime );
}

