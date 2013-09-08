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

// Standard library
#include <list>
#include <vector>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Responsible for queuing up changes requests and then issuing them to the system for modifying
///    to the correct state.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ChangeManager : public IChangeManager
{
public:

    ChangeManager( void );
    virtual ~ChangeManager( void );

    // Must be called after construction for a valid Change Manager

    // IChangeManager Functionality
    Error Register( ISubject* pInSubject, 
        System::Changes::BitMask uInIntrestBits, 
        IObserver* pInObserver, 
        System::Types::BitMask observerIdBits = System::Types::All );
    Error Unregister( ISubject* pSubject, IObserver* pObserver );
    Error DistributeQueuedChanges( System::Types::BitMask Systems2BeNotified = System::Types::All,
                                   System::Changes::BitMask ChangesToDist = System::Changes::All );

    // IObserver Functionality
    Error ChangeOccurred( ISubject* pInChangedSubject,
                          System::Changes::BitMask uInChangedBits );

    // Must be called before any parallel execution starts (that is 
    // before changes start being accumulated in thread local lists), 
    // but after the task manager has been initialized
    static void InitThreadLocalData( void* mgr );

    static void FreeThreadLocalData( void* mgr );


protected:

    /// <summary>
    ///   Defines a structure used by the CCM to store information about observers
    /// </summary>
    class ObserverRequest
    {
    public:
        ObserverRequest(
            IObserver* pObserver = nullptr,
            std::uint32_t Interests = 0,
            std::uint32_t idBits = CSubject::InvalidID
            )
            : m_pObserver( pObserver )
            , m_interestBits( Interests )
            , m_observerIdBits ( idBits )
        {
        }

        IObserver*      m_pObserver;
        std::uint32_t   m_interestBits;
        std::uint32_t   m_observerIdBits;

        bool operator < ( const ObserverRequest& rhs ) const
        {
            return m_pObserver < rhs.m_pObserver;
        }

        bool operator == ( const IObserver* rhs ) const
        {
            return m_pObserver == rhs;
        }
    }; // class ChangeManager::ObserverRequest

    typedef std::vector<ObserverRequest> ObserversList;

    /// <summary>
    ///   Represents a list of observers extended with cumulative data. 
    /// </summary>
    struct SubjectInfo
    {
        SubjectInfo ()
            : m_pSubject(nullptr)
            , m_interestBits(0)
            , m_observersList()
        {}

        /// <summary>
        ///   Subject described by this data structure 
        /// </summary>
        ISubject*       m_pSubject;

        /// <summary>
        ///   Cumulative interest bits of all observers from this list
        /// </summary>
        u32     m_interestBits;

        /// <summary>
        ///   Observers subscribed to this subject
        /// </summary>
        ObserversList   m_observersList;

    }; // class ChangeManager::ObserversList

    typedef std::vector<u32> IDsList;

    /// <summary>
    ///   List of IDs that become free after the subjects associated with them were unregistered
    /// </summary>
    IDsList             m_freeIDsList;

    /// <summary>
    ///   Next ID value to be assigned to a newly registered observer, if free list is empty
    /// </summary>
    u32                 m_lastID;

    typedef std::vector<SubjectInfo> SubjectsList;

    /// <summary>
    ///   Vector of observers list indexed by subject ID.
    /// </summary>
    SubjectsList        m_subjectsList;

    struct Notification
    {
        Notification ( ISubject* pSubject, u32 changedBits )
            : m_pSubject(pSubject)
            , m_changedBits(changedBits)
        {}

        ISubject*   m_pSubject;
        u32         m_changedBits;
    };
public:    
    typedef std::vector<Notification> NotifyList;
protected:
    typedef  std::list<NotifyList*>  ListOfNotifyLists;

    /// <summary>
    ///   Cross-thread list of notification lists. Accessed only from the main thread.
    /// </summary>
    ListOfNotifyLists   m_NotifyLists;

    /// <summary>
    ///   TLS slot that store pointers to NotifyListInfo values, containing pointer 
    ///   to thread local notification lists with fast search ability and .
    /// </summary>
    static __thread_local NotifyList*  m_tlsNotifyList;

    struct MappedNotification
    {
        MappedNotification ( u32 uID, u32 changedBits )
            : m_subjectID(uID)
            , m_changedBits(changedBits)
        {}

        u32 m_subjectID;
        u32 m_changedBits;
    };

    typedef std::vector<MappedNotification> MappedNotifyList;

    /// <summary>
    ///   Cumulative list of notifications posted during the last execution stage.
    ///   Used during distribution stage only.
    /// </summary>
    MappedNotifyList    m_cumulativeNotifyList;

    /// <summary>
    ///   Used while generating cumulative list during the distribution stage.
    /// </summary>
    IDsList             m_indexList;

    std::mutex          m_UpdateMutex;

private:
    Error RemoveSubject ( ISubject* pSubject );

    static void DistributionCallback( void *param, u32 begin, u32 end );
    void DistributeRange ( u32 begin, u32 end );

    System::Types::BitMask      m_systems2BeNotified;
    System::Changes::BitMask    m_ChangesToDist;
};
