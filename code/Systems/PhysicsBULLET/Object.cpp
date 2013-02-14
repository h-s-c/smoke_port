// Core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
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
