//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/Object.hpp"
#include "Systems/AudioFMOD/Scene.hpp"
#include "Systems/AudioFMOD/System.hpp"
#include "Systems/AudioFMOD/Task.hpp"
//external
#include "fmod.hpp"

extern ManagerInterfaces g_Managers;

FMODTask::FMODTask(
    FMODScene* pScene
    )
    : ISystemTask( pScene )
    , m_pScene( pScene )
{
    ASSERT( m_pScene != NULL );
    m_pSystem = (FMODSystem*)m_pScene->GetSystem();
    m_Pause = False;
}


FMODTask::~FMODTask(
    void
    )
{
}


System::Type
FMODTask::GetSystemType(
    void
    )
{
    return System::Types::Audio;
}


void
FMODTask::Update(
    f32 DeltaTime
    )
{
    if ( m_pSystem->AudioInitialized() )
    {
        // Update - process sound changes (random sounds)
        m_pScene->Update( DeltaTime );
        m_Result = m_pSystem->m_pFMOD->update();
        if (m_Result != FMOD_OK)
        {
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        }

        // Check if environment is paused
        Bool Pause = g_Managers.pEnvironment->Runtime().GetStatus() == IEnvironment::IRuntime::Status::Paused;

        if( Pause && !m_Pause )
        {
            // Pause the audio
            m_Pause = True;
            FMODSystem::PauseAll();
        }
        else if( !Pause && m_Pause )
        {
            // Unpause the audio
            m_Pause = False;
            FMODSystem::ResumeAll();
        }

        // Capture CPU Utilization information
        f32 dsp, stream, geometry, update, total;
        m_Result = m_pSystem->m_pFMOD->getCPUUsage(&dsp, &stream, &geometry, &update, &total );
        if (m_Result != FMOD_OK)
        {
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        }
        //Debug::Print("s = %1.2f, d = %1.2f, m = %1.2f, c = %1.2f\n", stream, dsp, main, cpu);

        m_pSystem->SetCPUUsage( total );
    }
}

