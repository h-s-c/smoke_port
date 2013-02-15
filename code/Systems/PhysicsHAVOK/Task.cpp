// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Extern
#include <Common\Base\hkBase.h>
#include <Common\Base\Memory\Memory\Pool\hkPoolMemory.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Physics\Dynamics\Entity\hkpEntityListener.h>
#include <Physics\Dynamics\Entity\hkpEntityActivationListener.h>
#include <Physics\Dynamics\Phantom\hkpSimpleShapePhantom.h>
#include <Physics\Utilities\CharacterControl\CharacterProxy\hkpCharacterProxy.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
#ifdef __HAVOK_VDB__
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#endif
// System
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"
#include "Systems/PhysicsHAVOK/ObjectCharacter.hpp"
#include "Systems/PhysicsHAVOK/Task.hpp"
#include "Systems/PhysicsHAVOK/ServiceCollision.hpp"
#include "Systems/PhysicsHAVOK/System.hpp"

extern ManagerInterfaces    g_Managers;


///////////////////////////////////////////////////////////////////////////////
// HavokPhysicsTask - Constructor
HavokPhysicsTask::HavokPhysicsTask(
    HavokPhysicsScene* pScene,
    hkpWorld* pWorld
    )
    : ISystemTask( pScene )
    , m_pScene( pScene )
    , m_pWorld( pWorld )
{
    ASSERT( m_pScene != NULL );
    ASSERT( m_pWorld != NULL );

    m_pWorld->addEntityListener( this );

    // should we use data parallelism?
    Bool bUseThreads = g_Managers.pEnvironment->Variables().GetAsBool( "Physics::Parallel", True );
    bUseThreads = ( bUseThreads && ( g_Managers.pTask != NULL ) );
    if( bUseThreads)
    {
        m_cJobs = g_Managers.pTask->GetRecommendedJobCount();
    }
    else
    {
        m_cJobs = 1;
    }

#ifdef __HAVOK_VDB__
    m_pWorld->markForWrite();

    //
    // Create the physics context for the visual debugger.
    //
    m_pPhysicsContext = new hkpPhysicsContext;
    m_pPhysicsContext->addWorld( m_pWorld );
    ASSERT( m_pPhysicsContext != NULL );

    hkpPhysicsContext::registerAllPhysicsProcesses();

    //
    // Create the visual debugger.
    //
    hkArray<hkProcessContext*> Contexts;
    Contexts.pushBack( m_pPhysicsContext );

    m_pVisualDebugger = new hkVisualDebugger( Contexts );
    ASSERT( m_pVisualDebugger != NULL );

    m_pVisualDebugger->serve();
    m_pVisualDebugger->addDefaultProcess( "Shapes" );
    m_pVisualDebugger->addDefaultProcess( "Broadphase" );
    m_pVisualDebugger->addDefaultProcess( "Islands" );
    m_pVisualDebugger->addDefaultProcess( "CentreOfMass" );
    m_pVisualDebugger->addDefaultProcess( "Phantom" );

    m_pWorld->unmarkForWrite();
#endif
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokPhysicsTask - Destructor
HavokPhysicsTask::~HavokPhysicsTask(
    void
    )
{
    m_pWorld->markForWrite();
    m_pWorld->removeEntityListener( this );

#ifdef __HAVOK_VDB__
    m_pVisualDebugger->removeReference();
    m_pPhysicsContext->removeReference();
#endif

    m_pWorld->unmarkForWrite();
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Task
System::Type
HavokPhysicsTask::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// SetObjectActivation - Add or removes the given object from actively tracked objects
void
HavokPhysicsTask::SetObjectActivation(
    HavokPhysicsObject* pObject,
    Bool bActivated
    )
{
    if ( bActivated )
    {
        if ( !pObject->m_bStatic )
        {
            //
            // Make sure it's not already in the list.
            //
#ifdef _DEBUG
            for ( std::list<HavokPhysicsObject*>::iterator it=m_ActiveObjects.begin();
                  it != m_ActiveObjects.end(); it++ )
            {
                if ( *it == pObject )
                {
                    return;
                }
            }
#endif

            m_ActiveObjects.push_back( pObject );

            m_pWorld->markForWrite();
            pObject->m_pBody->addEntityActivationListener( this );
            m_pWorld->unmarkForWrite();
        }
    }
    else
    {
        m_ActiveObjects.remove( pObject );

        m_pWorld->markForWrite();
        pObject->m_pBody->removeEntityActivationListener( this );
        m_pWorld->unmarkForWrite();
    }
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update the system (this is were all the work gets done)
void
HavokPhysicsTask::Update(
    f32 DeltaTime
    )
{
    //
    // Make sure that the time step is greater than 0.
    //
    if ( DeltaTime > 0.0f )
    {
        if ( DeltaTime > 0.04f )
        {
            DeltaTime = 0.04f;
        }

        //
        // Iterate through all the active object and clear our their contact information.
        //
        for ( std::list<HavokPhysicsObject*>::iterator it=m_ActiveObjects.begin();
              it != m_ActiveObjects.end(); it++ )
        {
            (*it)->m_aIntersectionInfo.clear();
        }

        //
        // Copy the delta time.
        //
        m_DeltaTime = DeltaTime;

        //
        // Get the thread information.
        //
        m_PrimaryThreadId = ::GetCurrentThreadId();
        m_PrimaryThreadToken = m_pWorld->getThreadToken();

        //
        // Start the stepping.
        //
        m_pWorld->stepBeginSt( DeltaTime );

        if( m_cJobs > 1 )
        {
            //
            // Issue jobs for multi-threaded stepping.
            //
            g_Managers.pTask->ParallelFor(
                this,
                reinterpret_cast<ITaskManager::ParallelForFunction>(StepUpdateS),
                this,
                0,
                m_cJobs
            );
        }
        else
        {
            // just call methods directly
            StepUpdateS( this, 0, 1 );
        }

        //
        // End the stepping.
        //
        UpdateCompletion();
    }
}


///////////////////////////////////////////////////////////////////////////////
// StepUpdate - Advance the world owned by this thread
void
HavokPhysicsTask::StepUpdate(
    u32 ThreadId
    )
{
    //
    // Get the token for this thread.
    //
    hkpThreadToken Token =
        (ThreadId == m_PrimaryThreadId) ? m_PrimaryThreadToken : m_pWorld->getThreadToken();

    //
    // Step the world.
    //
    m_pWorld->stepProcessMt( Token );
}


///////////////////////////////////////////////////////////////////////////////
// UpdateCompletion - Callback function called when all threads have finished
void
HavokPhysicsTask::UpdateCompletion(
    void
    )
{
    //
    // End the world stepping.
    //
    m_pWorld->stepEndSt();

    //
    // Reset the thread tokens.
    //
    m_pWorld->resetThreadTokens();

    //
    // Iterate through all the active objects and have them update themselves
    //  and post their changes.
    //
    std::list<HavokPhysicsObject*>& ActiveObjects = m_ActiveObjects;

    for ( std::list<HavokPhysicsObject*>::iterator it=ActiveObjects.begin();
          it != ActiveObjects.end(); it++ )
    {
        (*it)->Update();
    }

    m_pWorld->lock();

    //
    // Process collision request (need to make this use multithreaded jobs)
    //
    HavokPhysicsSystem* pSystem = (HavokPhysicsSystem*)m_pScene->GetSystem();
    pSystem->GetService()->ProcessRequests( m_pScene );

    //
    // Process contact tracking per subject then observer.
    //
    for ( HavokPhysicsScene::CollisionTrackerIt itSubject=m_pScene->m_CollisionTracker.begin();
          itSubject != m_pScene->m_CollisionTracker.end(); itSubject++ )
    {
        hkpRigidBody* pBody = itSubject->first->m_pBody;

        //
        // Get the body information for the contact.
        //
        hkVector4 Position = pBody->getPosition();
        hkQuaternion Rotation = pBody->getRotation();
        hkVector4 LinearVelocity = pBody->getLinearVelocity();
        hkVector4 AngularVelocity = pBody->getAngularVelocity();
        hkAabb AABB;
        pBody->getCollidable()->getShape()->getAabb( hkTransform::getIdentity(), 0.0f, AABB );


        //
        // Create the contact information for the subject.
        //
        IIntersectionObject::Info IntersectionInfo;

        IntersectionInfo.pszName = itSubject->first->GetName();
        Position.store3( &IntersectionInfo.Position.x );
        Rotation.getImag().store3( &IntersectionInfo.Orientation.x );
        IntersectionInfo.Orientation.w = Rotation.getReal();
        LinearVelocity.store3( IntersectionInfo.LinearVelocity );
        AngularVelocity.store3( IntersectionInfo.AngularVelocity );
        AABB.m_min.store3( IntersectionInfo.AABBMin );
        AABB.m_max.store3( IntersectionInfo.AABBMax );
        

        //
        // Iterate through all the observers for this subject.
        //
        for ( std::vector<HavokPhysicsObject*>::iterator itObserver=itSubject->second.begin();
              itObserver != itSubject->second.end(); itObserver++ )
        {
            //
            // Add the contact information to the observer object.
            //
            (*itObserver)->AddIntersection( IntersectionInfo );
        }
    }

    //
    // Process character proxies 
    //
    std::list<HavokCharacterObject*> CharacterObjects = m_pScene->GetCharacters();

    for( std::list<HavokCharacterObject*>::iterator it = CharacterObjects.begin(); it != CharacterObjects.end(); it++ )
    {
        (*it)->Update( m_DeltaTime );
    }

#ifdef __HAVOK_VDB__
    if ( m_pVisualDebugger != NULL )
    {
        m_pVisualDebugger->step();
    }
#endif

    m_pWorld->unlock();
}


///////////////////////////////////////////////////////////////////////////////
// StepUpdateS - Advance the world owned by this thread (static version)
void
HavokPhysicsTask::StepUpdateS(
    HavokPhysicsTask* pTask,
    u32 uStart,
    u32 uEnd
    )
{
    UNREFERENCED_PARAM( uStart );
    UNREFERENCED_PARAM( uEnd );

    //
    // Call non-static version for given pTask
    //
    pTask->StepUpdate( ::GetCurrentThreadId() );
}


///////////////////////////////////////////////////////////////////////////////
// entityAddedCallback - Callback for when an entity is added by Havok
void
HavokPhysicsTask::entityAddedCallback(
    hkpEntity* pEntity
    )
{
    HavokPhysicsObject* pObject = reinterpret_cast<HavokPhysicsObject*>(pEntity->getUserData());
    //ASSERT( pObject != NULL );

    if ( pObject != NULL && !pObject->m_bStatic )
    {
        m_ActiveObjects.push_back( pObject );

        pEntity->addEntityActivationListener( this );
    }
}


///////////////////////////////////////////////////////////////////////////////
// entityRemovedCallback - Callback for when an entity is removed by Havok
void
HavokPhysicsTask::entityRemovedCallback(
    hkpEntity* pEntity
    )
{
    HavokPhysicsObject* pObject = reinterpret_cast<HavokPhysicsObject*>(pEntity->getUserData());
    ASSERT( pObject != NULL );

    if ( !pObject->m_bStatic )
    {
        m_ActiveObjects.remove( pObject );

        pEntity->removeEntityActivationListener( this );
    }
}


///////////////////////////////////////////////////////////////////////////////
// entityActivatedCallback - Callback for when an entity is actived by Havok
void
HavokPhysicsTask::entityActivatedCallback(
    hkpEntity* pEntity
    )
{
    HavokPhysicsObject* pObject = reinterpret_cast<HavokPhysicsObject*>(pEntity->getUserData());
    ASSERT( pObject != NULL );

    m_ActiveObjects.push_back( pObject );
}


///////////////////////////////////////////////////////////////////////////////
// entityDeactivatedCallback - Callback for when an entity is deactived by Havok
void
HavokPhysicsTask::entityDeactivatedCallback(
    hkpEntity* pEntity
    )
{
    HavokPhysicsObject* pObject = reinterpret_cast<HavokPhysicsObject*>(pEntity->getUserData());
    ASSERT( pObject != NULL );

    pObject->m_aIntersectionInfo.clear();

    m_ActiveObjects.remove( pObject );
}
