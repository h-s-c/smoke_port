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

Scheduler::Scheduler()
{

    m_bBenchmarkingEnabled = EnvironmentManager::getInstance().Variables().GetAsBool( "Scheduler::Benchmarking", False );
    m_bThreadingEnabled = EnvironmentManager::getInstance().Variables().GetAsBool( "Scheduler::Parallel", False );
}

void
Scheduler::SetScene(
    const UScene* pScene
    )
{

#if 0
    // Wait for any executing scenes to finish and clear out the list.
    std::vector <ISystemTask*> aScenesToWaitFor;
    for (const auto &it : m_SceneExecs)
    {
        ISystemScene* pSystemScene = it.second;        
        aScenesToWaitFor.push_back(pSystemScene->GetSystemTask());
    }
    m_SceneExecs.clear();

    if ( aScenesToWaitFor.size() > 0 )
    {
        TaskManager::getInstance().WaitForSystemTasks( aScenesToWaitFor );
    }
#endif /* 0 */

    // Copy over all the system scenes.
    const UScene::SystemScenes& SystemScenes = pScene->GetSystemScenes();

    for ( UScene::SystemScenesConstIt it = SystemScenes.begin();
          it != SystemScenes.end(); it++ )
    {
        // Make sure the system has a task.
        if ( it->second->GetSystemTask() != nullptr )
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
    if (!m_bBenchmarkingEnabled)
    {
        m_Akkumulator += DeltaTime;
        if( m_Akkumulator < (1.0f / 120.0f))
        {
            return;
        }
        else
        {
            DeltaTime = m_Akkumulator;
            m_Akkumulator = 0.0f;
        }
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
        std::vector<ISystemTask*> aScenesToExecute;

        for (const auto &it : m_SceneExecs)
        {
            ISystemScene* pSystemScene = it.second;
            ISystemTask*  pSystemTask = pSystemScene->GetSystemTask();
            aScenesToExecute.push_back(pSystemTask);
        }

        TaskManager::getInstance().IssueJobsForSystemTasks( aScenesToExecute, DeltaTime );

#if 0
        // Wait for the scenes that will be completing execution in this frame.
        std::vector<ISystemTask*>  aScenesToWaitFor;
        for (const auto &it : m_SceneExecs)
        {
            ISystemScene* pSystemScene = it->second;
            aScenesToWaitFor.push_back(pSystemScene->GetSystemTask());
        }

        TaskManager::getInstance().WaitForSystemTasks( aScenesToWaitFor );
#endif /* 0 */
        TaskManager::getInstance().WaitForSystemTasks( aScenesToExecute );
    }
    else
    {
        for (const auto &it : m_SceneExecs)
        {
            ISystemScene* pSystemScene = it.second;
            pSystemScene->GetSystemTask()->Update( DeltaTime );
        }
    }
}
