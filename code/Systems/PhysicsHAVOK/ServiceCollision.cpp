// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
#include "Systems/PhysicsHAVOK/ServiceCollision.hpp"
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"
// Extern
#include <Common\Base\hkBase.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Dynamics\World\hkpWorldObject.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Physics\Collide\Query\CastUtil\hkpWorldRayCastInput.h>
#include <Physics\Collide\Query\CastUtil\hkpWorldRayCastOutput.h>
#include <Physics\Collide\Query\Collector\RayCollector\hkpClosestRayHitCollector.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>

//
// global variables
//
extern ManagerInterfaces    g_Managers;


//
// local defines
//
#define SCOPE_LOCK( x ) SpinWait::Lock Lock##x( x )


//
// local prototypes
//
static void ProcessColl( CollData& Data, HavokPhysicsScene* pScene );
static void LineTest( const Coll::Request& Request, Coll::Result* Result, HavokPhysicsScene* pScene );


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
    m_CollData.clear();

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
        SCOPE_LOCK( m_DeadHandlesLock );
        SCOPE_LOCK( m_PendingResultsLock );

        if( !m_DeadHandles.empty() )
        {
            std::vector<Coll::Handle>::iterator it;
            for( it = m_DeadHandles.begin(); it != m_DeadHandles.end(); it++ )
            {
                m_CollData.erase( *(it) );
                m_PendingResults.erase( *(it) );
            }

            m_DeadHandles.clear();
        }
    }

    // Add all new request
    {
        SCOPE_LOCK( m_PendingRequestsLock );
        if( !m_PendingRequests.empty() )
        {
            std::vector<Coll::Request>::iterator it;
            for( it = m_PendingRequests.begin(); it != m_PendingRequests.end(); it++ )
            {
                // Build an entry
                CollData Data;
                memset( &Data, 0, sizeof( Data ) );

                Data.m_Request = (*it);

                m_CollData[ (*it).m_Handle ] = Data;
            }

            m_PendingRequests.clear();
        }
    }

    // Process all outstanding requests
    if( !m_CollData.empty() )
    {
        u32 size = (u32)m_CollData.size();
        UNREFERENCED_PARAM( size );

        std::map<Coll::Handle,CollData>::iterator it;
        for( it = m_CollData.begin(); it != m_CollData.end(); it++ )
        {
            ProcessColl( (*it).second, pScene );

            // Store result
            {
                SCOPE_LOCK( m_PendingResultsLock );
                m_PendingResults[ (*it).second.m_Request.m_Handle ] = (*it).second.m_Result;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Test - Requests a collision test
Coll::Handle
HavokCollisionService::Test(
    const Coll::Request& Request
    )
{
    Coll::Handle Handle = Coll::InvalidHandle;

    // Create a new entry
    Coll::Request PendingRequest = Request;

    // Store data to run the test later
    {
        SCOPE_LOCK( m_PendingRequestsLock );
        
        Handle = GetNextHandle();
        PendingRequest.m_Handle = Handle;
    
        m_PendingRequests.push_back( PendingRequest );
    }

    // Return the handle
    return Handle;
}


///////////////////////////////////////////////////////////////////////////////
// LineTest - Requests a collision line test
Coll::Handle
HavokCollisionService::LineTest(
    const Math::Vector3& Start,
    const Math::Vector3& End,
    Coll::Request& Request
    )
{
    // Build request
    Request.m_Type      = Coll::e_LineTest;
    Request.m_Position0 = Start;
    Request.m_Position1 = End;

    // Register the test
    return Test( Request );
}


///////////////////////////////////////////////////////////////////////////////
// Finalize - Gets results for the given handle
Bool
HavokCollisionService::Finalize(
    Coll::Handle Handle,
    Coll::Result* pResult
    )
{
    ASSERT( pResult != NULL );
    ASSERT( Handle != Coll::InvalidHandle );

    // Return if the handle isn't valid
    if( Handle == Coll::InvalidHandle )
    {
        return False;
    }

    // Finalized will be returned if the test has completed
    Bool Finalized = False;

    // Get the data for the given handle
    Bool ResultAvailable = False;
    Coll::Result PendingResult;
    memset( &PendingResult, 0, sizeof( PendingResult ) );

    {
        SCOPE_LOCK( m_PendingResultsLock );

        std::map<Coll::Handle,Coll::Result>::iterator it = m_PendingResults.find( Handle );
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
            SCOPE_LOCK( m_DeadHandlesLock );
            m_DeadHandles.push_back( Handle );
        }
    }

    return Finalized;
}


///////////////////////////////////////////////////////////////////////////////
// GetNextHandle - Returns the next unique handle
Coll::Handle 
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
ProcessColl( 
    CollData& Data, 
    HavokPhysicsScene* pScene 
    )
{
    // Process the collision (if not already processed)
    if( !Data.m_Result.m_Finalized )
    {
        switch( Data.m_Request.m_Type )
        {
        case Coll::e_LineTest:
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
    const Coll::Request& Request, 
    Coll::Result* Result, 
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

