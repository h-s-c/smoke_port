//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//stdlib
#include <iostream>
//framework
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/Universal.hpp"
#include "Framework/TaskManager.hpp"
#include "Framework/Scheduler.hpp"

Scheduler::Scheduler( 
    TaskManager* pTaskManager )
    : m_pTaskManager( pTaskManager )
    , m_bThreadingEnabled( True )
{

    m_bBenchmarkingEnabled =
        EnvironmentManager::getInstance().Variables().GetAsBool( "Scheduler::Benchmarking", False );
}


Scheduler::~Scheduler(
    void
    )
{
}


void
Scheduler::SetScene(
    const UScene* pScene
    )
{
    //
    // If we have a TaskManager, then we can thread things.
    //
    m_bThreadingEnabled = ( m_pTaskManager != NULL );
   
    //
    // Wait for any executing scenes to finish and clear out the list.
    //
    ISystemTask* aScenesToWaitFor[ System::Types::MAX ];
    u32 cScenesToWaitFor = 0;

    for ( SceneExecsIt it=m_SceneExecs.begin(); it != m_SceneExecs.end(); it++ )
    {
        ISystemScene* pSystemScene = it->second;
        
        if ( cScenesToWaitFor > System::Types::MAX )
        {
                std::cerr << "cScenesToWaitFor > System::Types::MAX" << std::endl;
        }
        
        aScenesToWaitFor[ cScenesToWaitFor++ ] = pSystemScene->GetSystemTask();
    }
    m_SceneExecs.clear();

    if ( cScenesToWaitFor > 0 )
    {
        m_pTaskManager->WaitForSystemTasks( aScenesToWaitFor, cScenesToWaitFor );
    }

    //
    // Copy over all the system scenes.
    //
    const UScene::SystemScenes& SystemScenes = pScene->GetSystemScenes();

    for ( UScene::SystemScenesConstIt it = SystemScenes.begin();
          it != SystemScenes.end(); it++ )
    {
        //
        // Make sure the system has a task.
        //
        if ( it->second->GetSystemTask() != NULL )
        {
            m_SceneExecs[ it->first ] = it->second;
        }
    }

    //
    // Re-create the timer as a scene load may have taken a long time.
    //
    _start = std::chrono::high_resolution_clock::now();
}


void
Scheduler::Execute(
    void
    )
{
    // Get the delta time; seconds since last Execute call.
    auto DeltaTime = 0.0f;

    DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>( std::chrono::high_resolution_clock::now() - _start).count();
    
    // Check if the execution is paused, and set delta time to 0 if so.
    if ( EnvironmentManager::getInstance().Runtime().GetStatus() ==
         IEnvironment::IRuntime::Status::Paused )
    {
        DeltaTime = 0.0f;
    }

    if ( m_bThreadingEnabled )
    {
        // Schedule the scenes that are ready for execution.
        ISystemTask* aScenesToExecute[ System::Types::MAX ];
        u32 cScenesToExecute = 0;

        for ( SceneExecsIt it=m_SceneExecs.begin(); it != m_SceneExecs.end(); it++ )
        {
            if ( cScenesToExecute > System::Types::MAX )
            {
                std::cerr << "cScenesToExecute > System::Types::MAX" << std::endl;
            }
            ISystemScene* pSystemScene = it->second;
            aScenesToExecute[ cScenesToExecute++ ] = pSystemScene->GetSystemTask();
        }

        m_pTaskManager->IssueJobsForSystemTasks( aScenesToExecute, cScenesToExecute, DeltaTime );

        //
        // Wait for the scenes that will be completing execution in this frame.
        //
#if 0
        ISystemTask* aScenesToWaitFor[ System::Types::MAX ];
        u32 cScenesToWaitFor = 0;

        for ( SceneExecsIt it=m_SceneExecs.begin(); it != m_SceneExecs.end(); it++ )
        {
            if ( cScenesToWaitFor > System::Types::MAX )
            {
                std::cerr << "cScenesToWaitFor > System::Types::MAX" << std::endl;
            }
            ISystemScene* pSystemScene = it->second;
            aScenesToWaitFor[ cScenesToWaitFor++ ] = pSystemScene->GetSystemTask();
        }

        m_pTaskManager->WaitForSystemTasks( aScenesToWaitFor, cScenesToWaitFor );
#endif /* 0 */
        m_pTaskManager->WaitForSystemTasks( aScenesToExecute, cScenesToExecute );
    }
    else
    {
        for ( SceneExecsIt it=m_SceneExecs.begin(); it != m_SceneExecs.end(); it++ )
        {
            ISystemScene* pSystemScene = it->second;
            pSystemScene->GetSystemTask()->Update( DeltaTime );
        }
    }
}
