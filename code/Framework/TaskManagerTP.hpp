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

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

class TaskManagerTP;
 
// our worker thread objects
class Worker {
public:
    Worker(TaskManagerTP &s) : pool(s) { }
    void operator()();
private:
    TaskManagerTP &pool;    
};

class TaskManagerTP : public TaskManager
{
public:

    /// <summary cref="TaskManagerTP::Init">
    /// Call this from the primary thread before calling any other <c>TaskManagerTP</c> methods.
    /// </summary>
    void Init( void );

    /// <summary cref="TaskManagerTP::Shutdown">
    /// Call this from the primary thread as the last <c>TaskManagerTP</c> call.
    /// </summary>
    void Shutdown( void );

    /* Call this from the primary thread to schedule system work.*/
    void IssueJobsForSystemTasks( std::vector<ISystemTask*> pTasks, float fDeltaTime );
    
    /// <summary cref="TaskManagerTP::WaitForAllSystemTasks">
    /// Call this from the primary thread to wait until all tasks spawned with <c>IssueJobsForSystemTasks</c>
    /// and all of their subtasks are complete.
    /// </summary>
    /// <seealso cref="TaskManagerTP::IssueJobsForSystemTasks"/>
    void WaitForAllSystemTasks( void );
    
    /* Call this from the primary thread to wait until all tasks spawned with IssueJobsForSystemTasks
     * and all of their subtasks are complete.*/
    void WaitForSystemTasks( std::vector<ISystemTask*> pTasks );

    /// <summary cref="TaskManagerTP::GetNumberOfThreads">
    /// Call this method to get the number of threads in the thread pool which are active for running work.
    /// </summary>
    /// <returns>the number of threads being used</returns>
    u32 GetNumberOfThreads( void );
    
    /// <summary cref="TaskManagerTP::SetNumberOfThreads">
    /// This method constrains the number of threads used by the <c>TaskManagerTP</c>.
    /// </summary>
    /// <param name="uNumberOfThreads">the limit of the number of threads to use</param>
    void SetNumberOfThreads( u32 uNumberOfThreads );

    /// <summary cref="TaskManagerTP::NonStandardPerThreadCallback">
    /// This method triggers a synchronized callback to be called once by each thread used by the <c>TaskManagerTP</c>.
    /// This method which should only be called during initialization and shutdown of 
    /// the <c>TaskManagerTP</c>.  This method waits until all callbacks have executed.
    /// </summary>
    /// <param name="pfnCallback">the function callback to execute</param>
    /// <param name="pData">a pointer to data that is passed to the callback</param>
    virtual void NonStandardPerThreadCallback( JobFunction pfnCallback, void* pData );

    /// <summary cref="TaskManagerTP::GetRecommendedJobCount">
    /// Call this method to determine the ideal number of tasks to submit to the <c>TaskManagerTP</c>
    /// for maximum performance.
    /// </summary>
    /// <remarks><paramref name="Hints"/> is ignored and the number of active threads is always returned.</remarks>
    /// <param name="Hints">guidance on the type of work done in the jobs about to be submitted</param>
    /// <returns>the number of jobs which is optimal for the type of work specified by <paramref name="Hints"/>
    /// </returns>
    virtual u32 GetRecommendedJobCount( ITaskManager::JobCountInstructionHints Hints );

    virtual void ParallelFor( ISystemTask* pSystemTask,
                              ParallelForFunction pfnJobFunction, void* pParam, u32 begin, u32 end, u32 minGrainSize = 1 );

private:
    friend class Worker;

    uint32_t uNumberOfThreads;

    // need to keep track of threads so we can join them
    
    std::vector< std::thread > workers;
 
    // the task queue
    std::deque< std::function<void()> > tasks;
 
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    
    std::function<void()> callback;
    std::vector<std::thread::id> callbackDone;            

    template<class F>
    void Enqueue(F f)
    {
        { // acquire lock
            std::unique_lock<std::mutex> lock(this->queue_mutex);
     
            // add the task
            this->tasks.push_back(std::function<void()>(f));
        } // release lock
     
        // wake up one thread
        this->condition.notify_one();
    }    
    void Start();
    void Stop();
};
