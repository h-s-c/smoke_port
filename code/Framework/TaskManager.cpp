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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/TaskManager.hpp"
#include <iostream>

std::once_flag                   
TaskManager::only_one;
std::shared_ptr<TaskManager> 
TaskManager::instance_ = nullptr;

void Worker::operator()()
{
    std::function<void()> task = nullptr;
    while(true)
    {       
        {   // acquire lock
            std::unique_lock<std::mutex> lock(this->pool.queue_mutex);
                       
            auto thisThreadId = std::this_thread::get_id();
            // check if we already executed the callback on this thread
            bool callbackExecuted = false;
            for(const auto &threadid : this->pool.callbackDone)
            {
                if (threadid == thisThreadId)
                {
                    callbackExecuted = true;
                }
            }
            
            if (callbackExecuted == false && this->pool.callback)
            {
                // excute callback
                this->pool.callback();
                // add this thread to called back list
                this->pool.callbackDone.push_back(thisThreadId);
            }
            
            // look for a work item
            while(!this->pool.stop && this->pool.tasks.empty() && (this->pool.workers.size() == this->pool.callbackDone.size())  )
            { 
                // if there are none wait for notification
                std::this_thread::yield();
                this->pool.condition.wait_for(lock, std::chrono::duration<int, std::milli>(5));
            }
            
            // exit if the pool is stopped
            if(this->pool.stop) 
                return;

            // get the task from the queue
            if (!this->pool.tasks.empty())
            {
                task = std::move(this->pool.tasks.front());
                this->pool.tasks.pop_front();
            }
        }   // release lock
 
        // execute the task
        if (task != nullptr)
        {
            task();
        }
    }
}

TaskManager::TaskManager()
{
    auto numThreads = EnvironmentManager::getInstance().Variables().GetAsInt( "TaskManager::Threads", 0 );
    if( numThreads <= 0 ) 
    {
        this->numThreads = std::thread::hardware_concurrency();
    } 
    else
    {
        this->numThreads = numThreads;
    }
    
    // start worker threads
    this->stop = false;
    this->workers.clear();    
    for(size_t i = 0;i<this->numThreads;++i)
        this->workers.push_back(std::move(std::thread(Worker(*this))));
}

TaskManager::~TaskManager()
{
    // stop all threads
    this->stop = true;
    this->condition.notify_all();
    WaitForAllTasks();
 
    // join them
    for(auto & worker : this->workers)
    {
        worker.join();
    }
}

void TaskManager::EnqueueTasks( std::vector<ISystemTask*> systemTasks, float deltaTime)
{
    std::vector<ISystemTask*> notThreadSafeTasks;
    for (const auto &task : systemTasks)
    {
        if(task->IsThreadSafe())
        {
            this->AddTask([task, deltaTime] () { task->Update(deltaTime); });
        }
        else
        {
            notThreadSafeTasks.push_back(task);
        }        
    }    
    for (const auto &task : notThreadSafeTasks)
    {
        task->Update(deltaTime);  
    }
}

void TaskManager::WaitForAllTasks()
{
    auto empty = false;
    while(!empty)
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        empty = this->tasks.empty();
    }
}

void TaskManager::PerThreadCallback( JobFunction pfnCallback, void* pData)
{
    WaitForAllTasks();
    
    { // acquire lock
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        
        // clear called back list
        this->callbackDone.clear();
        
        // add the callback
        this->callback = std::bind(pfnCallback, pData);
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
}

void TaskManager::ParallelFor(
    ISystemTask* pSystemTask,
    ParallelForFunction pfnJobFunction, 
    void* pParam, 
    u32 begin, 
    u32 end, 
    u32 minGrainSize
    )
{
    u32 uThreads = ( this->numThreads + 1 );
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
            this->AddTask([pfnJobFunction, pParam, uStart, uEnd] () { pfnJobFunction( pParam, uStart, uEnd); });
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
