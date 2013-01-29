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

// Temporarily switching warning level to 0 to ignore warnings with the STL list that originate in conflicts in extern/Ogre 
#pragma warning( push, 0 )
#include "..\..\BaseTypes\BaseTypes.h"
#pragma warning( pop )

#include "..\..\Interfaces\Interface.h"
#include "..\..\Interfaces\Services\CollisionAPI.h"
#include "..\..\Framework\ServiceManager.h"

//
// System includes
//
#include "..\..\SystemInput\Object.h"

// Temporarily switching warning level to 0 to ignore warnings in extern/Ogre and SystemGraphicsOgre's extern/Ogre
#pragma warning( push, 0 )
#include "OgreSceneNode.h"
#pragma warning( pop )

#include "..\..\SystemGraphicsOgre\Object.h"
#include "..\..\SystemGraphicsOgre\Task.h"
#include "..\..\SystemGraphicsOgre\ObjectCamera.h"

//
// WaterStream system includes
//
#include "..\System.h"
#include "..\Scene.h"

#include "WaterStream.h"

#include <windows.h>

#pragma warning( push, 0 )
#pragma warning( disable : 6011 )

#define MAX_WATERSTREAM_LIFE 1.2f
//#define POSITION_UNDERGROUND Math::Vector3::Vector3( 0.0f, -2000.0f, 0.0f )
#define WATER_VELOCITY 3000.0f

// Local data
pcstr WaterStream::sm_kapszPropertyNames[] =
{
    "Unused", // Properties are TBD for this object
};

const Properties::Property WaterStream::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Unused ], // Placeholder property for this object (currently unused)
    VALUE2( Properties::Values::Vector3, Properties::Values::String ),
    Properties::Flags::Valid | Properties::Flags::Multiple,
    NULL, NULL, NULL, NULL,
    "", 0.0f, 0.0f, 0.0f ),
};


///////////////////////////////////////////////////////////////////////////////
// WaterStream - Constructor
WaterStream::WaterStream( ISystemScene* pSystemScene, pcstr pszName, int iStreamObjectID ) : WaterObject( pSystemScene, pszName )
{
    // Set default values
    m_ObjectID = iStreamObjectID;
    m_bSprayWater = false;
    m_bRepositionWater = false;
    m_Position = Math::Vector3::Zero;
    m_Velocity = Math::Vector3::Zero;
}


///////////////////////////////////////////////////////////////////////////////
// ~WaterStream - Destructor
WaterStream::~WaterStream( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occured (change handler)
Error WaterStream::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    m_bRepositionWater = false;

    if( ChangeType & System::Changes::Input::Firehose )
    {
        InputObject* pInputObj = dynamic_cast<InputObject*>(pSubject);
        if( pInputObj )
        {
            if( m_bSprayWater )
            {
                // Turn it off
                m_bSprayWater = false;
            }
            else
            {
                // Turn it on
                m_bRepositionWater = true;
                m_WaterStreamLifetime = 0.0f;
                m_bSprayWater = true;
            }
        }
    }

    if( ChangeType & System::Changes::Geometry::Position )
    {
        OGREGraphicsObjectCamera* pOgreCamObj = dynamic_cast<OGREGraphicsObjectCamera*>(pSubject);
        if( pOgreCamObj )
        {
            m_CameraPosition = *pOgreCamObj->GetPosition();
        }
    }

    if( ChangeType & System::Changes::Geometry::Orientation )
    {
        OGREGraphicsObjectCamera* pOgreCamObj = dynamic_cast<OGREGraphicsObjectCamera*>(pSubject);
        if( pOgreCamObj )
        {
            m_CameraOrientation = *pOgreCamObj->GetOrientation();
        }
    }

    if( ChangeType & System::Changes::POI::Contact )
    {
        IContactObject* pContactObject = dynamic_cast<IContactObject*>(pSubject);
        ASSERT( pContactObject );

        // If the water ball contacted a static object (meshes) reposition it below the ground.
        // This will force the update() to reposition the water ball to the camera and assign it
        // a new velocity, preventing frame updates from drawing between point of contact and
        // the camera.
        if( pContactObject->GetContact()->m_Static )
        {
            m_bRepositionWater = true;
        }
    }

    // Call parent handler
    Error Result = WaterObject::ChangeOccurred( pSubject, ChangeType );

    return Result;
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void WaterStream::Update( f32 DeltaTime )
{
    // Call base Update
    WaterObject::Update( DeltaTime );

    // Don't update for the first 10 frames so Havok can be properly initialized
    static int UpdateDelay = 0;
    if( UpdateDelay < 10 )
    {
        UpdateDelay++;
        return;
    }


    // Update the water
    if( m_bSprayWater )
    {
        m_WaterStreamLifetime += DeltaTime;

        // Only fire the water ball if the fire hose is on, the object contacted a static mesh,
        // or if the object has not collided with a static mesh after a period of time.
        if( m_bRepositionWater || (m_WaterStreamLifetime > MAX_WATERSTREAM_LIFE) )
        {
            m_WaterStreamLifetime = 0.0f;

            // Position the water at the camera and with an upwards velocity
            m_Position = m_CameraPosition;
            Math::Vector3 m_Translation( 0.0f, -5.0f, 0.0f ); // Apply a translation as a point of origin

            m_CameraOrientation.Rotate( m_Translation );
            m_Position += m_Translation;

            m_Velocity = -Math::Vector3::UnitZ;
            m_CameraOrientation.Rotate( m_Velocity ); // Assign a slight upwards velocity into the screen         
            m_Velocity *= WATER_VELOCITY; // Assign enough of a magnitude to launch the ball away from the camera

            PostChanges( System::Changes::Geometry::Position | System::Changes::Physics::Velocity );
        }
    }
    else
    {
        // Reposition the ball after it has been turned off at the camera's position and zero out the velocity to keep it in place
        m_Position = m_CameraPosition;
        Math::Vector3 m_Translation( 0.0f, -5.0f, 0.0f ); // Apply a translation as a point of origin

        m_CameraOrientation.Rotate( m_Translation );
        m_Position += m_Translation;

        m_Velocity = Math::Vector3::Zero;

        PostChanges( System::Changes::Geometry::Position | System::Changes::Physics::Velocity );
    }
}


///////////////////////////////////////////////////////////////////////////////
void WaterStream::SetVelocity(Math::Vector3 velocity)
{
    m_Velocity = velocity;
}


///////////////////////////////////////////////////////////////////////////////
const Math::Vector3* WaterStream::GetVelocity(void)
{
    return &m_Velocity;
}


///////////////////////////////////////////////////////////////////////////////
const Math::Vector3* WaterStream::GetPosition(void)
{
    return &m_Position;
}

///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns systems this Scene is interested in
System::Changes::BitMask WaterStream::GetDesiredSystemChanges( void )
{
    return( System::Changes::Input::Firehose | System::Changes::POI::Contact | 
        System::Changes::Geometry::Position | System::Changes::Geometry::Orientation );
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this WaterStream
System::Changes::BitMask WaterStream::GetPotentialSystemChanges( void )
{
    return ( System::Changes::Geometry::Position | System::Changes::Physics::Velocity );
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Get properties for this WaterStream
void WaterStream::GetProperties( Properties::Array& Properties )
{
    // Add all the properties.
    Properties.reserve( Properties.size() + WaterStream::Property_Count );

    for( i32 i = 0; i < WaterStream::Property_Count; i++ )
    {
        Properties.push_back( WaterStream::sm_kaDefaultProperties[ i ] );
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this WaterStream
void WaterStream::SetProperties( Properties::Array Properties )
{
    // Read in the properties
    for( Properties::Iterator it = Properties.begin(); it != Properties.end(); it++ )
    {
        ASSERT( False );  // No properties currently exist for this object
    }
}

#pragma warning( pop )
