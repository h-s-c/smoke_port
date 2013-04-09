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
#include "Systems/PhysicsBULLET/Object.hpp"


///////////////////////////////////////////////////////////////////////////////
// BulletObject - Constructor
BulletObject::BulletObject(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : ISystemObject( pSystemScene, pszName )
    , m_Position( Base::Vector3::Zero )
    , m_Orientation( Base::Quaternion::Zero )
    , m_Scale( Base::Vector3::One )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~BulletObject - Destructor
BulletObject::~BulletObject(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Object
System::Type
BulletObject::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the position of this Object
const Base::Vector3*
BulletObject::GetPosition(
    void
    )
{
    return &m_Position;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the orientation of this Object
const Base::Quaternion*
BulletObject::GetOrientation(
    void
    )
{
    return &m_Orientation;
}


///////////////////////////////////////////////////////////////////////////////
// GetScale - Returns the scale of this Object
const Base::Vector3*
BulletObject::GetScale(
    void
    )
{
    ASSERT( False );
    return NULL;
}
