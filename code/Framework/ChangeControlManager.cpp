// Copyright � 2008-2009 Intel Corporation
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

// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interfaces
#include "Interfaces/Interface.hpp"
// Standard library
#include <algorithm>
#include <utility>
#include <iostream>
#include <thread>
// Framework
#include "Framework/ChangeControlManager.hpp"
#include "Framework/PlatformManager.hpp"

__thread ChangeManager::NotifyList* ChangeManager::m_tlsNotifyList = nullptr;

///////////////////////////////////////////////////////////////////////////////
// ChangeManager - Default constructor 
ChangeManager::ChangeManager(
    void
    )
    : m_lastID(0)
    , m_pTaskManager(nullptr)
{
    // Get ready to process changes in the main (this) thread
    InitThreadLocalData(this);
}


///////////////////////////////////////////////////////////////////////////////
// ~ChangeManager - Default destructor
ChangeManager::~ChangeManager(
    void
    )
{
    // Loop through all the subjects and their observers and clean up 
    SubjectsList::iterator it;
    for( it = m_subjectsList.begin(); it != m_subjectsList.end(); ++it )
    {
        if ( it->m_pSubject )
        {
            ObserversList &observersList = it->m_observersList;

            ObserversList::iterator obsIt;
            for( obsIt = observersList.begin(); obsIt != observersList.end(); ++obsIt )
            {
                Unregister(it->m_pSubject, obsIt->m_pObserver);
            }
        }
    }

    // Free tls (thread local storage) data
    FreeThreadLocalData(this);
}



///////////////////////////////////////////////////////////////////////////////
// Register - Register a new subject/observer relationsship
Error
ChangeManager::Register(
    ISubject* pInSubject, 
    System::Changes::BitMask observerIntrestBits,
    IObserver* pInObserver,
    System::Types::BitMask observerIdBits
    )
{
    Error curError = Errors::Failure;
    if( pInSubject && pInObserver )
    {
        // Lock out updates while we register a subjext
        std::lock_guard<std::mutex> lock(m_UpdateMutex);

        std::uint32_t uID = pInSubject->GetID(this);

        if( uID != CSubject::InvalidID )
        {
            // Subject has already been registered. Add new observer to the list
            SubjectInfo &si = m_subjectsList[uID];
            if( si.m_pSubject != pInSubject)
            {
                std::cerr << "m_subjectsList[uID].m_pSubject != pInSubject" << std::endl;
            }
            else
            {
                si.m_observersList.push_back( ObserverRequest(pInObserver, observerIntrestBits, observerIdBits) );
                observerIntrestBits &= ~si.m_interestBits;
                if( observerIntrestBits )
                {
                    si.m_interestBits |= observerIntrestBits;
                    pInSubject->UpdateInterestBits(this, observerIntrestBits);
                }
            }
        }
        else 
        { 
            // New subject
            if ( m_freeIDsList.empty() )
            {
                // No zero ID should ever be assigned, so use pre-increment
                uID = ++m_lastID;
                m_subjectsList.resize( uID + 1 );
            }
            else
            {
                uID = m_freeIDsList.back();
                m_freeIDsList.pop_back();
            }
            if( uID == CSubject::InvalidID )
            {
                std::cerr << "uID == CSubject::InvalidID" << std::endl;
            }
            SubjectInfo &si = m_subjectsList[uID];
            si.m_pSubject = pInSubject;
            si.m_observersList.push_back( ObserverRequest(pInObserver, observerIntrestBits, observerIdBits) );
            si.m_interestBits = observerIntrestBits;

            pInSubject->Attach(this, observerIntrestBits, uID);
        }
        curError = Errors::Success;
    }
    return curError;
}


///////////////////////////////////////////////////////////////////////////////
// Unregister - Remove a subject/observer relationsship
Error
ChangeManager::Unregister(
    ISubject* pInSubject,
    IObserver* pInObserver
    )
{
    Error curError = Errors::Failure;

    if( pInSubject && pInObserver )
    {
        std::lock_guard<std::mutex> lock(m_UpdateMutex);

        u32 uID = pInSubject->GetID(this);
        if ( m_subjectsList.size() <= uID  ||  m_subjectsList[uID].m_pSubject != pInSubject )
        {
            return Errors::Failure;
        }
        if( m_subjectsList[uID].m_pSubject != pInSubject)
        {
            std::cerr << "m_subjectsList[uID].m_pSubject != pInSubject" << std::endl;
        }

        ObserversList &observersList = m_subjectsList[uID].m_observersList;
        ObserversList::iterator itObs = std::find( observersList.begin(), observersList.end(), pInObserver );

        if( itObs != observersList.end() )
        {
            observersList.erase(itObs);
            if( observersList.empty() )
            {
                m_subjectsList[uID].m_pSubject = nullptr;
                m_freeIDsList.push_back(uID);
                pInSubject->Detach(this);
            } 
            curError = Errors::Success;
        }
    } 
    return curError;
}


///////////////////////////////////////////////////////////////////////////////
// RemoveSubject - Remove a subject
Error 
ChangeManager::RemoveSubject ( 
    ISubject* pSubject
    )
{
    Error curError = Errors::Undefined;

    ObserversList observersList;
    // create a block to scope a lock
    {
        std::lock_guard<std::mutex> lock(m_UpdateMutex);

        u32 uID = pSubject->GetID(this);
        if( uID == CSubject::InvalidID)
        {
            std::cerr << "ChangeManager::RemoveSubject - uID == CSubject::InvalidID" << std::endl;
        }
        if( m_subjectsList[uID].m_pSubject != pSubject)
        {
            std::cerr << "ChangeManager::RemoveSubject - m_subjectsList[uID].m_pSubject != pSubject" << std::endl;
        }

        if ( m_subjectsList.size() <= uID  ||  m_subjectsList[uID].m_pSubject != pSubject )
        {
            return Errors::Failure;
        }
        observersList = m_subjectsList[uID].m_observersList;
        m_subjectsList[uID].m_pSubject = nullptr;
        m_freeIDsList.push_back(uID);
        curError = Errors::Success;
    }

    ObserversList::iterator itObs = observersList.begin();
    for( ; itObs != observersList.end(); ++itObs )
    {
        pSubject->Detach( itObs->m_pObserver );
    }

    return curError;
}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Process a change.  This stores all information needed to 
//                  process the change when DistributeQueuedChanges is called.
Error
ChangeManager::ChangeOccurred(
    ISubject* pInChangedSubject,
    System::Changes::BitMask uInChangedBits
    )
{
    Error curError = Errors::Undefined;

    if(pInChangedSubject == nullptr)
    {
        std::cerr << "ChangeManager::ChangeOccurred - pInChangedSubject == nullptr" << std::endl;
    }
    else
    {
        if(!uInChangedBits)
        {
            // The subject is shutting down - remove its reference
            curError = RemoveSubject( pInChangedSubject );
        }
        else
        {
            // Get thread local notification list
            NotifyList* notifyList = this->m_tlsNotifyList;
            
            // IMPLEMENTATION NOTE
            // Don't check for duplicate instertions
            //
            // For the sake of performance and scalability don't do any operations 
            // that may require locks (requesting ID or checking shared data structures).
            // Frequent locking hurts incomparably more than even high percentage 
            // of duplicated insertions, especially taking into account that the memory 
            // is preallocated most of the time.
            notifyList->push_back( Notification(pInChangedSubject, uInChangedBits) );
            curError = Errors::Success;           
        }
    }

    return curError;
} 


///////////////////////////////////////////////////////////////////////////////
// DistributeQueuedChanges - Distribute all queued notifications to the proper observers
Error
ChangeManager::DistributeQueuedChanges(
    System::Types::BitMask systems2BeNotified,
    System::Changes::BitMask ChangesToDist
    )
{
    // Store the parameters so they can be used by multiple threads later
    m_systems2BeNotified = systems2BeNotified;
    m_ChangesToDist = ChangesToDist;

    // Loop through all the notifications.  We might need to loop through multiple
    // times because processing notifications might generate more notifications
    for(;;)
    {
        // Make sure m_indexList is big enough to hold all subjects
        m_indexList.resize( m_subjectsList.size() );

        // Loop through all list and build m_cumulativeNotifyList
        for( auto &pList : m_NotifyLists)
        {
            size_t nOrigSize = pList->size();
            for( size_t i = 0; i < nOrigSize; i++ )
            {
                // Get notification
                Notification &notif = pList->at(i);

                // Get subject for notification
                auto uID = notif.m_pSubject->GetID(this);
                if( uID == CSubject::InvalidID )
                {
                   std::cerr << "ChangeManager::DistributeQueuedChanges - uID == CSubject::InvalidID" << std::endl;
                }
                else
                {
                    // Get the index for this subject
                    auto index = m_indexList[uID];

                    // If index is set, then this subject is already part of the m_cumulativeNotifyList
                    if( index )
                    {
                        // Each subject only needs to be notified once for all changes
                        // so let's combine all notifications for this subject
                        m_cumulativeNotifyList[ index ].m_changedBits |= notif.m_changedBits;
                    }
                    else
                    {
                        // Set the index for this subject
                        m_indexList[uID] = (std::uint32_t)m_cumulativeNotifyList.size();

                        // Add a new entry to m_cumulativeNotifyList
                        m_cumulativeNotifyList.push_back( MappedNotification(uID, notif.m_changedBits) );
                    }
                }
            }

            // Clear out this list
            pList->clear();
        }

        // Determine number of notifications to process
        size_t NumberOfChanges = m_cumulativeNotifyList.size();

        // Exit the loop if we don't have any messages to process
        if( NumberOfChanges == 0 )
        {
            break;
        }

        // If we have a task manager and there are more than 50 notifications to process, let's do it parallel
        static const u32 GrainSize = 50;
        if( !m_pTaskManager && (u32)NumberOfChanges > GrainSize )
        {
            // Process noticitions in parallel
            m_pTaskManager->ParallelFor( nullptr, DistributionCallback, this, 0, (u32)NumberOfChanges, GrainSize );
        }
        else
        {
            // Not enough notifications to worry about running in parallel, just distribute in this thread
            DistributeRange( 0, (u32)NumberOfChanges );
        }

        // Check if we are distributing all the notifications
        if( m_ChangesToDist == System::Changes::All )
        {
            // If we are distributing all the notifications, clear out m_cumulativeNotifyList
            m_cumulativeNotifyList.clear();
            m_indexList.clear();
        }
        else
        {
            // Some of the notifications might need to be distributed later
            // Keep m_cumulativeNotifyList and exit the loop
            break;
        }
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// DistributionCallback - This callback is used to divide notifications 
//                        distribution among multiple threads
void 
ChangeManager::DistributionCallback( 
    void *param, 
    u32 begin, 
    u32 end 
    )
{
    // Process the given range (this will be called from multiple threads)
    ChangeManager* pMgr = static_cast<ChangeManager*>(param);
    pMgr->DistributeRange( begin, end );
}


///////////////////////////////////////////////////////////////////////////////
// DistributeRange - Process queued notifications for the given range
void 
ChangeManager::DistributeRange( 
    u32 begin, 
    u32 end 
    )
{
    // Loop through all the noticatiosn in the given range
    for ( size_t i = begin; i < end; ++i )
    {
        // Get the notification and the subject
        MappedNotification &notif = m_cumulativeNotifyList[ i ];
        SubjectInfo &subject = m_subjectsList[ notif.m_subjectID ];

        // Distribute any desired changes
        u32 activeChanges = notif.m_changedBits & m_ChangesToDist;
        if( activeChanges )
        {
            // Clear the bit for the changes we are distributing
            notif.m_changedBits &= ~activeChanges;

            // Loop through all the observers and let them process the notifictaion
            ObserversList &obsList = subject.m_observersList;
            for( size_t j = 0; j != obsList.size(); ++j )
            {
                // Determine if this observe is interested in this notification
                u32 changesToSend = obsList[j].m_interestBits & activeChanges;
                if ( changesToSend )
                {
                    // If this observer is part of the systems to be notified then we can pass it this notification
                    if( obsList[j].m_observerIdBits & m_systems2BeNotified )
                    {
                        // Have the observer process this change (notification)
                        obsList[j].m_pObserver->ChangeOccurred(subject.m_pSubject, changesToSend);
                    }
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetTaskManager - Set TaskManager and init associated data
Error 
ChangeManager::SetTaskManager( 
    ITaskManager* pTaskManager 
    )
{
    if ( !pTaskManager )
    {
        std::cerr << "ChangeManager::SetTaskManager - !pTaskManager" << std::endl;
        return Errors::Undefined;
    }

    if( m_pTaskManager )
    {
        std::cerr << "ChangeManager::SetTaskManager - Call ResetTaskManager before using SetTaskManager to set the new task manager" << std::endl;
    }

    // Store TaskManager
    m_pTaskManager = pTaskManager;

    // Make each thread call InitThreadLocalData
    m_pTaskManager->NonStandardPerThreadCallback( InitThreadLocalData, this );
    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// ResetTaskManager - Init thread specific data
// (Must be called before the previously set task manager has been shut down)
void ChangeManager::ResetTaskManager(
    void
    )
{
    // Free all data associated with m_pTaskManager
    if ( m_pTaskManager )
    {
        // Make each thread call FreeThreadLocalData
        m_pTaskManager->NonStandardPerThreadCallback( FreeThreadLocalData, this );
        m_NotifyLists.clear();
        m_pTaskManager = nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////
// InitThreadLocalData - Init thread specific data
void
ChangeManager::InitThreadLocalData( 
    void* arg 
    )
{  
    if( !arg )
    {
        std::cerr << "ChangeManager::InitThreadLocalData - No manager pointer passed to InitThreadLocalNotifyList" << std::endl;
    }
    ChangeManager *mgr = (ChangeManager*)arg;
    // Check if we have allocated a NotifyList for this thread.
    // The notify list is keep in tls (thread local storage).
    if( mgr->m_tlsNotifyList == nullptr)
    {
        mgr->m_tlsNotifyList = new NotifyList();
        // Lock out the updates and add this NotifyList to m_NotifyLists
        std::lock_guard<std::mutex> lock(mgr->m_UpdateMutex);
        mgr->m_NotifyLists.push_back(mgr->m_tlsNotifyList);
    }
    else
    {
        std::clog << "Thread local data already initialized on thread " << std::this_thread::get_id() << std::endl;
    }   
}


///////////////////////////////////////////////////////////////////////////////
// FreeThreadLocalData - Free thread specific data
void
ChangeManager::FreeThreadLocalData( 
    void* arg 
    )
{
    if( !arg )
    {
        std::cerr << "ChangeManager::FreeThreadLocalData - No manager pointer passed to FreeThreadLocalNotifyList" << std::endl;
    }
    
    ChangeManager *mgr = (ChangeManager*)arg;

    // Free tls (thread local storage) data
    if ( mgr->m_tlsNotifyList != nullptr )
    {
        // Free NotifyList if it exists
        delete mgr->m_tlsNotifyList;
        mgr->m_tlsNotifyList = nullptr;
    }
} 
