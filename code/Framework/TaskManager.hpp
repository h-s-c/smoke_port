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

// workaround for gcc4.7
#define _GLIBCXX_USE_SCHED_YIELD
#include <future>
#include <thread>
#include <deque>
#include <vector>
#include <utility>
#include <chrono>
#include <functional>
#include <type_traits>

/* Thanks to https://github.com/greyfade/workqueue.*/
class TaskManager;
class Worker 
{
public:
    Worker(TaskManager &s) : pool(s) { }
    void operator()();
private:
    TaskManager &pool;    
};

class TaskManager: public ITaskManager
{
private:
    // singleton
    static std::shared_ptr<TaskManager> instance_;
    static std::once_flag                   only_one;
     
    TaskManager(const TaskManager& rs) {
        instance_  = rs.instance_;
    }
 
    TaskManager& operator = (const TaskManager& rs) 
    {
        if (this != &rs) 
        {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    TaskManager();

public:
    static TaskManager& getInstance() 
    {
        std::call_once( TaskManager::only_one, [] () 
        { 
            TaskManager::instance_.reset( new TaskManager()); 
        });
 
        return *TaskManager::instance_;
    }
    
    ~TaskManager();

    /* Call this from the primary thread to schedule system work.*/
    void EnqueueTasks( std::vector<ISystemTask*> systemTasks, float deltaTime );
    
    /* Adds a task and returns a std::future*/
    template<class F>
    auto AddTask(std::packaged_task<F()>& task) -> std::future<F> 
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);

        auto ret = task.get_future();
        this->tasks.push_back([&task]{task();});

        this->condition.notify_one();

        return ret;
    }
    
    /* Adds a task.*/    
    template<class F>
    void AddTask(F f)
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        
        this->tasks.push_back(std::function<void()>(f));
        
        this->condition.notify_one();
    } 

    /* Call this from the primary thread to wait until all tasks and all of their subtasks are complete.*/
    void WaitForAllTasks();

    /* Call this method to get the number of threads in the thread pool which are active for running work.*/
    std::uint32_t GetNumberOfThreads() {return this->numThreads;}
                            
    /* This method triggers a synchronized callback to be called once by each thread used by the TaskManager. */
    virtual void PerThreadCallback( JobFunction pfnCallback, void* pData );

    /* Call this method to determine the ideal number of tasks to submit to the TaskManager 
     * for maximum performance.*/
    virtual std::uint32_t GetRecommendedJobCount( ITaskManager::JobCountInstructionHints Hints ) {return this->numThreads;}
    
    virtual void ParallelFor( ISystemTask* pSystemTask, ParallelForFunction pfnJobFunction, void* pParam, std::uint32_t begin, std::uint32_t end, std::uint32_t minGrainSize = 1 );

private:
    friend class Worker;

    // number of active threads
    std::uint32_t numThreads;

    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
 
    // the task queue
    std::deque<std::function<void()>> tasks;
 
    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    
    // per thread callback
    std::function<void()> callback;
    std::vector<std::thread::id> callbackDone;    
};
