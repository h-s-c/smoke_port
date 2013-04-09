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
#include <map>
#include <vector>

// VS2010 support
#if (COMPILER == "MSVC") && (COMPILER_VERSION == "10.0")
#include "External/tinythread/tinythread.h"
namespace std { using namespace tthread;}
#else
#include <mutex>
#endif

// Forward declarations
class HavokPhysicsScene;

struct CollisionData
{
    Collision::Request m_Request;    // Collision request
    Collision::Result  m_Result;     // Result of collision
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>HavokCollisionService</c> Implementation of the ICollision interface 
///   for HavokPhysics.  This service provides collision test to other systems.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class HavokCollisionService : public IService::ICollision
{
public:

    HavokCollisionService( void );
    ~HavokCollisionService( void );

    /// <summary cref="HavokCollisionService::SetWorld">
    ///   Set the Havok World used to test collisions.
    /// </summary>
    /// <param name="pWorld">Pointer Havok World.</param>
    void SetWorld( hkpWorld* pWorld );

    /// <summary cref="HavokCollisionService::ProcessRequests">
    ///   Processes all outstanding collision test requests.
    /// </summary>
    /// <param name="pScene">Pointer to HavokPhysics system.</param>
    void ProcessRequests( HavokPhysicsScene* pScene );

    /// <summary cref="HavokCollisionService::Test">
    ///   Implementation of the <c>ICollision::Test</c> function.
    ///   Registers a test request and returns a unique handle to make future requests.
    /// </summary>
    /// <returns>Coll::Handle - A unique handle for this registered test.</returns>
    /// <seealso cref="ICollision::Test"/>
    virtual Collision::Handle Test( const Collision::Request& Request );

    /// <summary cref="HavokCollisionService::LineTest">
    ///   Registers a line test and returns a unique handle to make future requests.
    /// </summary>
    /// <returns>Coll::Handle - A unique handle for this registered test.</returns>
    /// <seealso cref="HavokCollisionService::Test"/>
    virtual Collision::Handle LineTest( const Base::Vector3& Start, const Base::Vector3& End, Collision::Request& Request );

    /// <summary cref="HavokCollisionService::Finalize">
    ///   Implementation of the <c>ICollision::Finalize</c> function.
    ///   Request the results of a collision test.  If the test is not complete, 
    ///   this will return false.
    /// </summary>
    /// <returns>Coll::Handle - A unique handle for this registered test.</returns>
    /// <seealso cref="ICollision::Finalize"/>
    virtual Bool Finalize( Collision::Handle Handle, Collision::Result* pResult );


protected:

    /// <summary cref="HavokCollisionService::GetNextHandle">
    ///   Get the next unique collision handle.
    /// </summary>
    /// <returns>Coll::Handle - A unique handle.</returns>
    Collision::Handle GetNextHandle( void );

    hkpWorld*                                   m_pWorld;               // Havok world

    u32                                         m_HandleCount;          // Handle counter (next unique handle)
    std::map<Collision::Handle,CollisionData>   m_CollisionData;             // Collision system workspace
    std::vector<Collision::Request>             m_PendingRequests;      // Store pending collision tests
    std::map<Collision::Handle,Collision::Result>    m_PendingResults;       // Store pending results
    std::vector<Collision::Handle>              m_DeadHandles;          // Store a list of used (dead) results

    std::mutex                                  m_PendingRequestsLock;  // Lock for m_PendingRequests
    std::mutex                                  m_PendingResultsLock;   // Lock for m_PendingResults
    std::mutex                                  m_DeadHandlesLock;      // Lock for m_DeadHandles
};

