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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/PhysicsBULLET/ObjectCollision.hpp"
#include "Systems/PhysicsBULLET/Scene.hpp"
#include "Systems/PhysicsBULLET/System.hpp"


extern ManagerInterfaces    g_Managers;


///////////////////////////////////////////////////////////////////////////////
// BulletPhysicsSystem - Constructor
BulletPhysicsSystem::BulletPhysicsSystem(
    void
    )
    : ISystem()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~BulletPhysicsSystem - Destructor
BulletPhysicsSystem::~BulletPhysicsSystem(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr
BulletPhysicsSystem::GetName(
    void
    )
{
    return System::Names::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type
BulletPhysicsSystem::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error
BulletPhysicsSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void
BulletPhysicsSystem::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void
BulletPhysicsSystem::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}

///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene*
BulletPhysicsSystem::CreateScene(
    void
    )
{
    return new BulletPhysicsScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destroys the given Scene, free all associated resources
Error
BulletPhysicsSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    BulletPhysicsScene* pScene = reinterpret_cast<BulletPhysicsScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}
