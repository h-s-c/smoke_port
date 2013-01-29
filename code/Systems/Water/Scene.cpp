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
// core includes
//
#include "..\BaseTypes\BaseTypes.h"
#include "..\Interfaces\Interface.h"

//
// System includes
//
#include "..\SystemInput\Object.h"

//
// Water system includes
//
#include "Scene.h"
#include "Object.h"
#include "Task.h"

#include "Objects\WaterStream.h"

// Local Structures
struct ProcessData
{
    std::vector<WaterObject*> m_Objects;    // Water Objects to process
    f32                    m_DeltaTime;     // Delta time (needed for Update calls)
};


extern ManagerInterfaces   g_Managers;


// Prototypes
void ProcessWater( void* Data );
u32 JobsComplete( void* pScene );

///////////////////////////////////////////////////////////////////////////////
// WaterScene - Constructor
WaterScene::WaterScene( ISystem* pSystem ) : ISystemScene( pSystem ), m_pWaterTask( NULL )
{
    m_StreamObjectID = 0;
}


///////////////////////////////////////////////////////////////////////////////
// ~WaterScene - Destructor
WaterScene::~WaterScene( void )
{
	SAFE_DELETE( m_pWaterTask );

    // Free all the remaining objects.
    for( std::vector<WaterObject*>::iterator it = m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Scene
System::Type WaterScene::GetSystemType( void )
{
    return System::Types::Water;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Scene with the given properties
Error WaterScene::Initialize( std::vector<Properties::Property> Properties )
{
    ASSERT( !m_bInitialized );

    m_pWaterTask = new WaterTask( this );
    ASSERT( m_pWaterTask != NULL );

	m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Scene are returned in Properties
void WaterScene::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Scene
void WaterScene::SetProperties( Properties::Array Properties )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetObjectTypes - Get Object types for this Scene
pcstr* WaterScene::GetObjectTypes( void )
{
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// CreateObject - Create an Object with the given Name and Type
ISystemObject* WaterScene::CreateObject( pcstr pszName, pcstr pszType )
{
    UNREFERENCED_PARAM( pszType );

    ASSERT( m_bInitialized );

	// Create the Water object
	WaterObject* pObject = NULL;
	if( strcmp( pszType, "WaterStream" ) == 0 )
	{
		pObject = (WaterObject*)new WaterStream( this, pszName, m_StreamObjectID++ );
	}
	else
	{
		// Create a default Water object (this is probably an error, so assert)
		ASSERT( false );
		pObject = new WaterObject( this, pszName );
	}

    if ( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    return pObject;
}


///////////////////////////////////////////////////////////////////////////////
// DestroyObject - Destroys the given Object, removing it from the Scene
Error WaterScene::DestroyObject( ISystemObject* pSystemObject )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    // Cast to a WaterObject so that the correct destructor will be called.
    WaterObject* pObject = reinterpret_cast<WaterObject*>(pSystemObject);

    // Remove the object from the list and delete it
	u32 index = 0;
	for( std::vector<WaterObject*>::iterator it=m_Objects.begin(); it != m_Objects.end(); it++ )
    {
		if( *it == pObject )
		{
			m_Objects.erase( m_Objects.begin() + index );
			break;
		}
		index++;
    }

    SAFE_DELETE( pSystemObject );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemTask - Returns the task associated with this Scene
ISystemTask* WaterScene::GetSystemTask( void )
{
    return m_pWaterTask;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Scene
System::Changes::BitMask WaterScene::GetPotentialSystemChanges( void )
{
    return System::Changes::None;
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns systems this Scene is interested in
System::Changes::BitMask WaterScene::GetDesiredSystemChanges( void )
{
    return System::Changes::None;
    //return( System::Changes::Geometry::Position |
    //    System::Changes::POI::Target );
}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Process changes that occured by other systems/objects
Error WaterScene::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    UNREFERENCED_PARAM( pSubject );
    UNREFERENCED_PARAM( ChangeType );

	return Errors::Success;
}
