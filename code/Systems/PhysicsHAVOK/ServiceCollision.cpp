// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Extern
#include <Common\Base\hkBase.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Dynamics\World\hkpWorldObject.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Physics\Collide\Query\CastUtil\hkpWorldRayCastInput.h>
#include <Physics\Collide\Query\CastUtil\hkpWorldRayCastOutput.h>
#include <Physics\Collide\Query\Collector\RayCollector\hkpClosestRayHitCollector.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
// System
#include "Systems/Common/System.hpp"
#include "Systems/PhysicsHAVOK/ServiceCollision.hpp"
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"

//
// global variables
//
extern ManagerInterfaces    g_Managers;


//
// local prototypes
//
static void ProcessCollision( CollisionData& Data, HavokPhysicsScene* pScene );
static void LineTest( const Collision::Request& Request, Collision::Result* Result, HavokPhysicsScene* pScene );


///////////////////////////////////////////////////////////////////////////////
// HavokCollisionService - Default constructor
HavokCollisionService::HavokCollisionService(
    void
    )
    : m_pWorld( NULL )
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


///////////////////////////////////////////////////////////////////////////////
// ~HavokCollisionService - Default destructor
HavokCollisionService::~HavokCollisionService(
    void
    )
{
    g_Managers.pService->UnregisterCollisionProvider( this );
}


///////////////////////////////////////////////////////////////////////////////
// SetWorld - Sets HavokCollisionService internal Havok world 
void
HavokCollisionService::SetWorld(
    hkpWorld* pWorld
    )
{
    m_pWorld = pWorld;
}



///////////////////////////////////////////////////////////////////////////////
// ProcessRequests - Process all requested collisions
void 
HavokCollisionService::ProcessRequests( 
    HavokPhysicsScene* pScene 
    )
{
    // Delete all dead entries
    {
        std::lock_guard<std::mutex> lock( m_DeadHandlesLock );
        std::lock_guard<std::mutex> lock2( m_PendingResultsLock );

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
        std::lock_guard<std::mutex> lock( m_PendingRequestsLock );
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
        u32 size = (u32)m_CollisionData.size();
        UNREFERENCED_PARAM( size );

        std::map<Collision::Handle,CollisionData>::iterator it;
        for( it = m_CollisionData.begin(); it != m_CollisionData.end(); it++ )
        {
            ProcessCollision( (*it).second, pScene );

            // Store result
            {
                std::lock_guard<std::mutex> lock( m_PendingResultsLock );
                m_PendingResults[ (*it).second.m_Request.m_Handle ] = (*it).second.m_Result;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Test - Requests a collision test
Collision::Handle
HavokCollisionService::Test(
    const Collision::Request& Request
    )
{
    Collision::Handle Handle = Collision::InvalidHandle;

    // Create a new entry
    Collision::Request PendingRequest = Request;

    // Store data to run the test later
    {
        std::lock_guard<std::mutex> lock( m_PendingRequestsLock );
        
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
HavokCollisionService::LineTest(
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
HavokCollisionService::Finalize(
    Collision::Handle Handle,
    Collision::Result* pResult
    )
{
    ASSERT( pResult != NULL );
    ASSERT( Handle != Collision::InvalidHandle );

    // Return if the handle isn't valid
    if( Handle == Collision::InvalidHandle )
    {
        return False;
    }

    // Finalized will be returned if the test has completed
    Bool Finalized = False;

    // Get the data for the given handle
    Bool ResultAvailable = False;
    Collision::Result PendingResult;
    memset( &PendingResult, 0, sizeof( PendingResult ) );

    {
       std::lock_guard<std::mutex> lock( m_PendingResultsLock );

        std::map<Collision::Handle,Collision::Result>::iterator it = m_PendingResults.find( Handle );
        if( it != m_PendingResults.end() )
        {
            ResultAvailable = True;
            PendingResult = (*it).second;
        }
    }

    if( ResultAvailable )
    {
        // Get store the result
        Finalized = True;
        if( pResult )
        {
            *pResult = PendingResult;
        }
            
        // Mark for deletion
        {
            std::lock_guard<std::mutex> lock( m_DeadHandlesLock );
            m_DeadHandles.push_back( Handle );
        }
    }

    return Finalized;
}


///////////////////////////////////////////////////////////////////////////////
// GetNextHandle - Returns the next unique handle
Collision::Handle 
HavokCollisionService::GetNextHandle( 
    void 
    )
{
    m_HandleCount = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
    return m_HandleCount;
}


///////////////////////////////////////////////////////////////////////////////
// ProcessColl - Process and individual collision
static void 
ProcessCollision( 
    CollisionData& Data, 
    HavokPhysicsScene* pScene 
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
            ASSERT( False );  // Unsupported collision type
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
    HavokPhysicsScene* pScene 
    )
{
    ASSERT( Result );

    // Setup Havok ray cast
    hkpWorldRayCastInput input;
    input.m_from.set( Request.m_Position0.x, Request.m_Position0.y, Request.m_Position0.z );
    input.m_to.set( Request.m_Position1.x, Request.m_Position1.y, Request.m_Position1.z );
    
    ASSERT( input.m_from.isOk3() );
    ASSERT( input.m_to.isOk3() );

    // Perform ray cast
    hkpClosestRayHitCollector output;
    pScene->GetWorld()->castRay( input, output );

    // Process results
    if( output.hasHit() )
    {
        // Hit something
        Result->m_Valid = True;
        
        // Set m_Position
        hkVector4 IntersectionPointWorld;
        IntersectionPointWorld.setInterpolate4( input.m_from, input.m_to, output.getHit().m_hitFraction );
        IntersectionPointWorld.store3( Result->m_Position );

        // Set m_Normal
        output.getHit().m_normal.store3( Result->m_Normal );

        // Set m_Hit
        hkpWorldObject* pWorldObject = (hkpWorldObject*)output.getHit().m_rootCollidable->getOwner();
        HavokObject* pObject = (HavokObject*)pWorldObject->getUserData();
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
    Result->m_Finalized = True;
}

