// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
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


//
// Core includes
//
#include "..\BaseTypes\BaseTypes.h"
#include "..\Interfaces\Interface.h"



//
// Water system includes
//
#include "Object.h"

///////////////////////////////////////////////////////////////////////////////
// WaterObject - Constructor
WaterObject::WaterObject( ISystemScene* pSystemScene, pcstr pszName ) : ISystemObject( pSystemScene, pszName )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~WaterObject - Destructor
WaterObject::~WaterObject( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Object
System::Type WaterObject::GetSystemType( void )
{
    return System::Types::Water;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Object with the given properties
Error WaterObject::Initialize( std::vector<Properties::Property> Properties )
{
    // Set this set as initialized.
    m_bInitialized = True;

    // Set properties
    SetProperties( Properties );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Object are returned in Properties
void WaterObject::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Object
void WaterObject::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns the systems changes this Object 
//                           is intrested in hearing about
System::Types::BitMask WaterObject::GetDesiredSystemChanges( void )
{
    return ( System::Changes::None );
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)
Error WaterObject::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    ASSERT( m_bInitialized );

    UNREFERENCED_PARAM( pSubject );
    UNREFERENCED_PARAM( ChangeType );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Object
System::Changes::BitMask WaterObject::GetPotentialSystemChanges( void )
{
    return( System::Changes::None );
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the position of this Object
const Base::Vector3* WaterObject::GetPosition( void )
{
    return &m_Position;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition - Returns the orientation of this Object
const Base::Quaternion* WaterObject::GetOrientation( void )
{
    return &m_Orientation;
}


///////////////////////////////////////////////////////////////////////////////
// GetScale - Returns the scale of this Object
const Base::Vector3* WaterObject::GetScale( void )
{
    return &m_Scale;
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void WaterObject::Update( f32 DeltaTime )
{
    m_State.UpdateTime( DeltaTime );
}
