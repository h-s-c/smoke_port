// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
#include "Systems/PhysicsHAVOK/Object.hpp"


///////////////////////////////////////////////////////////////////////////////
// HavokObject - Constructor
HavokObject::HavokObject(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : ISystemObject( pSystemScene, pszName )
    , m_Position( Math::Vector3::Zero )
    , m_Orientation( Math::Quaternion::Zero )
    , m_Scale( Math::Vector3::One )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokObject - Destructor
HavokObject::~HavokObject(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Object
System::Type
HavokObject::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the position of this Object
const Math::Vector3*
HavokObject::GetPosition(
    void
    )
{
    return &m_Position;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the orientation of this Object
const Math::Quaternion*
HavokObject::GetOrientation(
    void
    )
{
    return &m_Orientation;
}


///////////////////////////////////////////////////////////////////////////////
// GetScale - Returns the scale of this Object
const Math::Vector3*
HavokObject::GetScale(
    void
    )
{
    ASSERT( False );
    return NULL;
}
