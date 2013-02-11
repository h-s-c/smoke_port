// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
#include "Systems/InputOIS/Scene.hpp"
#include "Systems/InputOIS/Object.hpp"
#include "Systems/InputOIS/Task.hpp"


InputTask::InputTask(
    InputScene* pScene
    )
    : ISystemTask( pScene )
    , m_pScene( pScene )
{
    ASSERT( m_pScene != NULL );

    ASSERT( sizeof( LONG ) == sizeof( i32 ) );
}


InputTask::~InputTask(
    void
    )
{
}


System::Type
InputTask::GetSystemType(
    void
    )
{
    return System::Types::Input;
}


void
InputTask::Update(
    f32 DeltaTime
    )
{
    m_pScene->InputUpdate(DeltaTime);
 
    return;
}

