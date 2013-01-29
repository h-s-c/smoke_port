#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"

#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"

#include "Framework/TaskManager.hpp"
#include "Framework/TaskManagerTP.hpp"


void Worker::operator()()
{
    std::function<void()> task;
    while(true)
    {
        {   // acquire lock
            std::unique_lock<std::mutex> lock(this->pool.queue_mutex);
 
           // look for a work item
            while(!this->pool.stop && this->pool.tasks.empty())
            { // if there are none wait for notification
                this->pool.condition.wait(lock);
            }
 
            if(this->pool.stop) // exit if the pool is stopped
                return;

            // get the task from the queue
            task = this->pool.tasks.front();
            this->pool.tasks.pop_front();
 
        }   // release lock
 
        // execute the task
        task();
    }
}

void TaskManagerTP::Init( void)
{
    if( this->uNumberOfThreads <= 0 ) 
    {
        this->uNumberOfThreads = EnvironmentManager::getInstance().Variables().GetAsInt( "TaskManager::Threads", 0 );
        if( this->uNumberOfThreads <= 0 ) 
        {
            this->uNumberOfThreads = std::thread::hardware_concurrency() - 1;
        } 
    } 
    for(uint32_t i = 0;i<this->uNumberOfThreads;++i)
        this->workers.push_back(std::thread(Worker(*this)));
}

void TaskManagerTP::Shutdown( void)
{
    // stop all threads
    this->stop = true;
    this->condition.notify_all();
 
    // join them
    for(size_t i = 0;i<this->workers.size();++i)
        this->workers[i].join();
}

void TaskManagerTP::IssueJobsForSystemTasks( ISystemTask** pTasks, u32 uCount, f32 DeltaTime)
{
    for ( u32 i=0; i < uCount; i++ )
    {
        this->enqueue([pTasks, i, DeltaTime]
            {
                pTasks[i]->Update(DeltaTime);
            });
    }
}

void TaskManagerTP::WaitForAllSystemTasks( void )
{
    while(!this->tasks.empty())
    {

    }
}

void TaskManagerTP::WaitForSystemTasks( ISystemTask** pTasks, u32 uCount )
{
    // NOTE: not properly implemented
    // currently waits for all tasks
    DBG_UNREFERENCED_PARAM( pTasks );
    DBG_UNREFERENCED_PARAM( uCount );
    WaitForAllSystemTasks();
}

void TaskManagerTP::NonStandardPerThreadCallback( JobFunction pfnCallback, void* pData)
{
    for ( u32 i=0; i < this->uNumberOfThreads; i++ )
    {
        this->enqueue([pfnCallback, pData]
            {
                pfnCallback( pData );
            });
    }
    // call it for ourself, too
    pfnCallback( pData );
}

uint32_t TaskManagerTP::GetRecommendedJobCount( ITaskManager::JobCountInstructionHints Hints)
{
    //
    // Ignoring hints for now and just returning the number of available threads.
    //
    UNREFERENCED_PARAM( Hints );

    return this->uNumberOfThreads;
}

uint32_t TaskManagerTP::GetNumberOfThreads( void)
{
    return this->uNumberOfThreads;
}

void TaskManagerTP::SetNumberOfThreads( uint32_t uNumberOfThreads)
{
    if (this->uNumberOfThreads < uNumberOfThreads)
    {
        for(uint32_t i = this->uNumberOfThreads;i<uNumberOfThreads;++i)
            this->workers.push_back(std::thread(Worker(*this)));

        this->uNumberOfThreads = uNumberOfThreads;
    }
    else if (this->uNumberOfThreads > uNumberOfThreads)
    {
        this->uNumberOfThreads = uNumberOfThreads;
        this->Shutdown();
        this->Init();
    }
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

    ASSERT( uThreads > 0 );
    ASSERT( uCount >= 0 );

    u32 uGrainSize = ( uCount / uThreads );

    if( uGrainSize < minGrainSize )
    {
        uGrainSize = minGrainSize;
        ASSERT( uGrainSize > 0 );

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
            this->enqueue([pfnJobFunction, pParam, uStart, uEnd]
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
