/* The MIT License (MIT)
 * Copyright (c) 2013 Kevin Schmidt
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once

#include "Base/Platform.hpp"
#include "Base/Math.hpp"

#include <cstdint>
#include <map>
#include <mutex>
#include <vector>

#include <btBulletCollisionCommon.h>


// Forward declarations
class BulletPhysicsScene;

struct CollisionData
{
    Collision::Request m_Request;    // Collision request
    Collision::Result  m_Result;     // Result of collision
};


// Implementation of the ICollision interface for Bullet physics.  
// This service provides collision test to other systems.

class BulletCollisionService : public IService::ICollision
{
public:

    BulletCollisionService( void );
    ~BulletCollisionService( void );

    // Processes all outstanding collision test requests.
    void ProcessRequests( BulletPhysicsScene* pScene );

    // Implementation of the ICollision::Test function.
    // Registers a test request and returns a unique handle to make future requests.
    virtual Collision::Handle Test( const Collision::Request& Request );

    // Registers a line test and returns a unique handle to make future requests.
    virtual Collision::Handle LineTest( const Base::Vector3& Start, const Base::Vector3& End, Collision::Request& Request );

    // Implementation of the ICollision::Finalize function.
    // Request the results of a collision test.  
    // If the test is not complete, this will return false.
    virtual bool Finalize( Collision::Handle Handle, Collision::Result* pResult );


protected:

    // Get the next unique collision handle.
    Collision::Handle GetNextHandle( void );

    std::unit32_t                                   m_HandleCount;          // Handle counter (next unique handle)
    std::map<Collision::Handle,CollisionData>       m_CollisionData;        // Collision system workspace
    std::vector<Collision::Request>                 m_PendingRequests;      // Store pending collision tests
    std::map<Collision::Handle,Collision::Result>   m_PendingResults;       // Store pending results
    std::vector<Collision::Handle>                  m_DeadHandles;          // Store a list of used (dead) results

    std::mutex                                      m_PendingRequestsLock;  // Lock for m_PendingRequests
    std::mutex                                      m_PendingResultsLock;   // Lock for m_PendingResults
    std::mutex                                      m_DeadHandlesLock;      // Lock for m_DeadHandles
};

