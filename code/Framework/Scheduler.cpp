// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

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
    
    m_OldTime = std::chrono::high_resolution_clock::now();
}


void
Scheduler::Execute(
    void
    )
{
    m_NewTime = std::chrono::high_resolution_clock::now();
    
    // Ticks per second;
    auto DeltaTime =std::chrono::duration<float, std::ratio<1>>(m_NewTime - m_OldTime).count();
    m_OldTime = m_NewTime;  
    
    // Force 120hz      
    m_Akkumulator += DeltaTime;
    if( m_Akkumulator < (1.0f / 120.0f))
    {
        return;
    }
    else
    {
        m_Akkumulator = 0.0f;
    }
    
    
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
