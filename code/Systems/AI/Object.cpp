//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Object.hpp"


///////////////////////////////////////////////////////////////////////////////
// AIObject - Constructor
AIObject::AIObject( ISystemScene* pSystemScene, pcstr pszName ) : ISystemObject( pSystemScene, pszName )
{
    m_Behavior = Interface::e_Behavior_None;
}


///////////////////////////////////////////////////////////////////////////////
// ~AIObject - Destructor
AIObject::~AIObject( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Object
System::Type AIObject::GetSystemType( void )
{
    return System::Types::AI;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Object with the given properties
Error AIObject::Initialize( std::vector<Properties::Property> Properties )
{
    // Set this set as initialized.
    m_bInitialized = True;

    // Set properties
    SetProperties( Properties );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Object are returned in Properties
void AIObject::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Object
void AIObject::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns the systems changes this Object 
//                           is intrested in hearing about
System::Types::BitMask AIObject::GetDesiredSystemChanges( void )
{
    return ( System::Changes::Geometry::Position
           | System::Changes::Geometry::Orientation
           | System::Changes::Geometry::Scale );
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)
Error AIObject::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    ASSERT( m_bInitialized );

    // Process change
    if( ChangeType & System::Changes::Geometry::Position )
    {
        m_Position = *dynamic_cast<IGeometryObject*>(pSubject)->GetPosition();
    }

    if( ChangeType & System::Changes::Geometry::Orientation )
    {
        m_Orientation = *dynamic_cast<IGeometryObject*>(pSubject)->GetOrientation();
    }

    if( ChangeType & System::Changes::Geometry::Scale )
    {
        m_Scale = *dynamic_cast<IGeometryObject*>(pSubject)->GetScale();
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Object
System::Changes::BitMask AIObject::GetPotentialSystemChanges( void )
{
    return ( System::Changes::Geometry::Position
           | System::Changes::Geometry::Orientation
           | System::Changes::AI::Behavior );
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the position of this Object
const Base::Vector3* AIObject::GetPosition( void )
{
    return &m_Position;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the orientation of this Object
const Base::Quaternion* AIObject::GetOrientation( void )
{
    return &m_Orientation;
}


///////////////////////////////////////////////////////////////////////////////
// GetScale - Returns the scale of this Object
const Base::Vector3* AIObject::GetScale( void )
{
    return &m_Scale;
}


///////////////////////////////////////////////////////////////////////////////
// GetBehavior - Returns the current behavior of this Object
Behavior AIObject::GetBehavior()
{
    return m_Behavior;
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void AIObject::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void AIObject::Update( f32 DeltaTime )
{
    m_State.UpdateTime( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void AIObject::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// SetBehavior - Set the current behavior of this AI
void AIObject::SetBehavior( Interface::Behavior Behavior )
{
    if( m_Behavior != Behavior )
    {
        m_Behavior = Behavior;
        PostChanges( System::Changes::AI::Behavior );
    }
}
