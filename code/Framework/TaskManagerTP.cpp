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
#include "Framework/TaskManager.hpp"
#include "Framework/TaskManagerTP.hpp"


void Worker::operator()()
{
    std::function<void()> task = nullptr;
    while(true)
    {       
        {   // acquire lock
            std::unique_lock<std::mutex> lock(this->pool.queue_mutex);
            
            // check if we already executed the callback on this thread
            bool callbackExecuted = false;
            for(const auto &threadid : this->pool.callbackDone)
            {
                if (threadid == std::this_thread::get_id())
                {
                    callbackExecuted = true;
                }
                
            }
            if (callbackExecuted == false && this->pool.callback)
            {
                // excute callback
                this->pool.callback();
                std::clog << "TaskManagerTP - Callback called on thread " << std::this_thread::get_id() << std::endl;
                // add this thread to called back list
                this->pool.callbackDone.push_back(std::this_thread::get_id());
            }
            
            // look for a work item
            while(!this->pool.stop && this->pool.tasks.empty() && (this->pool.workers.size() == this->pool.callbackDone.size())  )
            { // if there are none wait for notification
                this->pool.condition.wait(lock);
            }
 
            if(this->pool.stop) // exit if the pool is stopped
                return;

            // get the task from the queue
            if (!this->pool.tasks.empty())
            {
                task = this->pool.tasks.front();
                this->pool.tasks.pop_front();
            }
            else
            {
                task = nullptr;
            }
        }   // release lock
 
        // execute the task
        if (task != nullptr && task != NULL)
        {
            task();
        }
    }
}

void TaskManagerTP::Init( void)
{
    auto uNumberOfThreads = EnvironmentManager::getInstance().Variables().GetAsInt( "TaskManager::Threads", 0 );
    // SetNumberOfThreads calls Start()
    if( uNumberOfThreads <= 0 ) 
    {
        SetNumberOfThreads(std::thread::hardware_concurrency());
    } 
    else
    {
        SetNumberOfThreads(uNumberOfThreads);
    }
}

void TaskManagerTP::Shutdown( void)
{
    Stop();
 
    // join them
    for(size_t i = 0;i<this->workers.size();++i)
    {
        this->workers[i].join();
    }
}

void TaskManagerTP::IssueJobsForSystemTasks( std::vector<ISystemTask*> pTasks, float DeltaTime)
{
    for (auto task : pTasks)
    {
        this->Enqueue([task, DeltaTime]
            {
                task->Update(DeltaTime);
            });
    }
}

void TaskManagerTP::WaitForAllSystemTasks( void )
{
    auto empty = false;
    while(!empty)
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        empty = this->tasks.empty();
    }
}

void TaskManagerTP::WaitForSystemTasks( std::vector<ISystemTask*> pTasks )
{
    // NOTE: not properly implemented
    // currently waits for all tasks
    WaitForAllSystemTasks();
}

void TaskManagerTP::NonStandardPerThreadCallback( JobFunction pfnCallback, void* pData)
{
    WaitForAllSystemTasks();
    
    { // acquire lock
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        
        // clear called back list
        this->callbackDone.clear();
        
        // add the callback
        this->callback = std::bind(pfnCallback, pData);
        std::clog << "TaskManagerTP - Callback added" << std::endl;
    } // release lock
     
    // wait for workers to execute callback
    auto done = false;
    while(!done)
    {
        { // acquire lock
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            if (this->workers.size() != this->callbackDone.size()) 
            {
                done = false;
            }
            else
            {
                done = true;
            }
        }
        this->condition.notify_all();
    }
    // call it for ourself, too
    pfnCallback( pData );
    std::clog << "TaskManagerTP - Callback called on thread " << std::this_thread::get_id() << " (Primary)" << std::endl;
    
    std::clog << "TaskManagerTP - All Callbacks executed" << std::endl;
}

uint32_t TaskManagerTP::GetRecommendedJobCount( ITaskManager::JobCountInstructionHints Hints)
{
    //
    // Ignoring hints for now and just returning the number of available threads.
    //
    return this->uNumberOfThreads;
}

uint32_t TaskManagerTP::GetNumberOfThreads( void)
{
    return this->uNumberOfThreads;
}

void TaskManagerTP::SetNumberOfThreads( uint32_t uNumberOfThreads)
{
    Stop();
    this->uNumberOfThreads = uNumberOfThreads;
    Start();
}

void TaskManagerTP::ParallelFor(
    ISystemTask* pSystemTask,
    ParallelForFunction pfnJobFunction, 
    void* pParam, 
    u32 begin, 
    u32 end, 
    u32 minGrainSize
    )
{
    u32 uThreads = ( this->uNumberOfThreads + 1 );
    u32 uCount = ( end - begin );

    if ( uThreads <= 0 )
    {
        std::cerr << "uThreads <= 0" << std::endl;
    }
    if ( uCount <= 0 )
    {
        std::cerr << "uCount <= 0" << std::endl;
    }

    u32 uGrainSize = ( uCount / uThreads );

    if( uGrainSize < minGrainSize )
    {
        uGrainSize = minGrainSize;
        if ( uGrainSize <= 0 )
        {
            std::cerr << "uGrainSize <= 0" << std::endl;
        }

        uThreads = ( uCount / uGrainSize );
    }

    if( uThreads > 1 )
    {
        u32 uStart = 0;
        u32 uEnd = 0;

        // dispatch other jobs
        for( u32 t = 1; t < uThreads; t++ )
        {
            uEnd = ( uStart + uGrainSize );
            this->Enqueue([pfnJobFunction, pParam, uStart, uEnd]
                {
                    pfnJobFunction( pParam, uStart, uEnd);
                });
            uStart = uEnd;
        }

        // now do our job
        uEnd = uCount;
        pfnJobFunction( pParam, uStart, uEnd );
    }
    else
    {
        // just do it ourselves
        pfnJobFunction( pParam, begin, end );
    }
}

void TaskManagerTP::Stop( void)
{
    // stop all threads
    this->stop = true;
    this->condition.notify_all();
    WaitForAllSystemTasks();
}

void TaskManagerTP::Start( void)
{
    this->stop = false;
    this->workers.clear();    
    for(size_t i = 0;i<this->uNumberOfThreads;++i)
        this->workers.push_back(std::thread(Worker(*this)));
}
