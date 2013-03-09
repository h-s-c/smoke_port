// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Standard Library
#include <iostream>
#include <mutex>
// System
#include "Systems/PhysicsBULLET/Scene.hpp"
#include "Systems/PhysicsBULLET/ServiceCollision.hpp"
#include "Systems/PhysicsBULLET/Object.hpp"
#include "Systems/PhysicsBULLET/ObjectPhysics.hpp"
// External
#include "btBulletCollisionCommon.h"


// global variables
extern ManagerInterfaces    g_Managers;

// local prototypes
static void ProcessCollision( CollisionData& Data, BulletPhysicsScene* pScene );
static void LineTest( const Collision::Request& Request, Collision::Result* Result, BulletPhysicsScene* pScene );


// BulletCollisionService - Default constructor
BulletCollisionService::BulletCollisionService(
    void
    )
{
    g_Managers.pService->RegisterCollisionProvider( this );

    // Setup collision workspace
    m_HandleCount = 0;
    m_CollisionData.clear();

    // Setup pending request
    m_PendingRequests.clear();

    // Setup pending results
    m_PendingResults.clear();

    // Setup dead (used) results
    m_DeadHandles.clear();
}

// ~BulletCollisionService - Default destructor
BulletCollisionService::~BulletCollisionService(
    void
    )
{
    g_Managers.pService->UnregisterCollisionProvider( this );
}

// ProcessRequests - Process all requested collisions
void 
BulletCollisionService::ProcessRequests( 
    BulletPhysicsScene* pScene 
    )
{
    // Delete all dead entries
    {
        std::lock_guard<std::mutex dead(m_DeadHandlesLock);
        std::lock_guard<std::mutex pending(m_PendingResultsLock);

        if( !m_DeadHandles.empty() )
        {
            std::vector<Collision::Handle>::iterator it;
            for( it = m_DeadHandles.begin(); it != m_DeadHandles.end(); it++ )
            {
                m_CollisionData.erase( *(it) );
                m_PendingResults.erase( *(it) );
            }

            m_DeadHandles.clear();
        }
    }

    // Add all new request
    {
        std::lock_guard<std::mutex pending(m_PendingRequestsLock);
        if( !m_PendingRequests.empty() )
        {
            std::vector<Collision::Request>::iterator it;
            for( it = m_PendingRequests.begin(); it != m_PendingRequests.end(); it++ )
            {
                // Build an entry
                CollisionData Data;
                memset( &Data, 0, sizeof( Data ) );

                Data.m_Request = (*it);

                m_CollisionData[ (*it).m_Handle ] = Data;
            }

            m_PendingRequests.clear();
        }
    }

    // Process all outstanding requests
    if( !m_CollisionData.empty() )
    {
        std::uint32_t size = (std::uint32_t)m_CollisionData.size();

        std::map<Collision::Handle,CollisionData>::iterator it;
        for( it = m_CollisionData.begin(); it != m_CollisionData.end(); it++ )
        {
            ProcessCollision( (*it).second, pScene );

            // Store result
            {
                std::lock_guard<std::mutex pending(m_PendingResultsLock);
                m_PendingResults[ (*it).second.m_Request.m_Handle ] = (*it).second.m_Result;
            }
        }
    }
}

// Test - Requests a collision test
Collision::Handle
BulletCollisionService::Test(
    const Collision::Request& Request
    )
{
    Collision::Handle Handle = Collision::InvalidHandle;

    // Create a new entry
    Collision::Request PendingRequest = Request;

    // Store data to run the test later
    {
        std::lock_guard<std::mutex pending(m_PendingRequestsLock);
        
        Handle = GetNextHandle();
        PendingRequest.m_Handle = Handle;
    
        m_PendingRequests.push_back( PendingRequest );
    }

    // Return the handle
    return Handle;
}


///////////////////////////////////////////////////////////////////////////////
// LineTest - Requests a collision line test
Collision::Handle
BulletCollisionService::LineTest(
    const Base::Vector3& Start,
    const Base::Vector3& End,
    Collision::Request& Request
    )
{
    // Build request
    Request.m_Type      = Collision::e_LineTest;
    Request.m_Position0 = Start;
    Request.m_Position1 = End;

    // Register the test
    return Test( Request );
}


///////////////////////////////////////////////////////////////////////////////
// Finalize - Gets results for the given handle
Bool
BulletCollisionService::Finalize(
    Collision::Handle Handle,
    Collision::Result* pResult
    )
{
    if (pResult == NULL)
    {
        std::cerr << "pResult == NULL" << std::endl;
    }
    
    // Return if the handle isn't valid
    if( Handle == Collision::InvalidHandle )
    {
        std::cerr << "Handle == Collision::InvalidHandle" << std::endl;
        return False;
    }

    // Finalized will be returned if the test has completed
    bool finalized = false;

    // Get the data for the given handle
    bool resultAvailable = false;
    Collision::Result PendingResult;
    memset( &PendingResult, 0, sizeof( PendingResult ) );

    {
        std::lock_guard<std::mutex pending(m_PendingResultsLock);

        std::map<Collision::Handle,Collision::Result>::iterator it = m_PendingResults.find( Handle );
        if( it != m_PendingResults.end() )
        {
            resultAvailable = true;
            PendingResult = (*it).second;
        }
    }

    if( tesultAvailable )
    {
        // Get store the result
        finalized = true;
        if( pResult )
        {
            *pResult = PendingResult;
        }
            
        // Mark for deletion
        {
            std::lock_guard<std::mutex pending(m_DeadHandlesLock);
            m_DeadHandles.push_back( Handle );
        }
    }

    return finalized;
}

// GetNextHandle - Returns the next unique handle
Collision::Handle 
BulletCollisionService::GetNextHandle( 
    void 
    )
{
    m_HandleCount = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
    return m_HandleCount;
}

// ProcessCollision - Process and individual collision
static void 
ProcessCollision( 
    CollisionData& Data, 
    BulletPhysicsScene* pScene 
    )
{
    // Process the collision (if not already processed)
    if( !Data.m_Result.m_Finalized )
    {
        switch( Data.m_Request.m_Type )
        {
        case Collision::e_LineTest:
            LineTest( Data.m_Request, &Data.m_Result, pScene );
            break;

        default:
            std::cerr << "Unsupported collision type" << std::endl;
            break;
        }
    } 
}


///////////////////////////////////////////////////////////////////////////////
// LineTest - Initiate a collision line test
static void 
LineTest( 
    const Collision::Request& Request, 
    Collision::Result* Result, 
    BulletPhysicsScene* pScene 
    )
{
    if ( !pResult)
    {
        std::cerr << "!pResult" << std::endl;
    }
    
    btDiscreteDynamicsWorld* world = pScene.GetWorld()
    
    if (world )
    {  
        // Step the simulation
        world->updateAabbs();
        world->computeOverlappingPairs();

        // Perform ray cast
        btVector3 from(Request.m_Position0.x, Request.m_Position0.y, Request.m_Position0.z );
        btVector3 to(Request.m_Position1.x, Request.m_Position1.y, Request.m_Position1.z );    
        btCollisionWorld::ClosestRayResultCallback  closestResults(from,to);
        closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
        world->rayTest(from,to,closestResults);

        // Process results
        if (closestResults.hasHit())
        {
            
            btVector3 p = from.lerp(to,closestResults.m_closestHitFraction);
            sDebugDraw.drawSphere(p,0.1,blue);
            sDebugDraw.drawLine(p,p+closestResults.m_hitNormalWorld,blue);
            
            // Hit something
            Result->m_Valid = true;
            
            // Set m_Position
            btVector3 position = closestResults.m_hitPointWorld;
            Result->m_Position.x = position.x;
            Result->m_Position.y = position.y;
            Result->m_Position.z = position.z;

            // Set m_Normal
            btVector3 pos = closestResults.m_hitNormalWorld;
            Result->m_Normal.x = normal.x;
            Result->m_Normal.y = normal.y;
            Result->m_Normal.z = normal.z;

            // Set m_Hit
            BulletObject* pObject = (BulletObject*)closestResults.collisionObject.getUserPointer();
            if( pObject )
            {
                Result->m_Hit = pObject->GetName();
            }
        }
        else
        {
            // Didn't hit anything
            Result->m_Valid = False;
            Result->m_Position = Request.m_Position1;
        }
        // Mark is as finalized
        Result->m_Finalized = true; 
    }
    else
    {
        std::cerr << "no btDiscreteDynamicsWorld to work on" << std::endl;
    }
}

