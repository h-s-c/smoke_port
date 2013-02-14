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

#pragma once

//
// core includes
//
#include "..\..\Interfaces\Services\CollisionAPI.h"

//
// WaterStream system includes
//
#include "..\Object.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>WaterStream</c> Base class for WaterStream objects.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class WaterStream : public WaterObject, public IMoveObject
{
public:

    /// <summary>
    /// ctor
    /// </summary>
    /// <param name="pSystemScene">Pointer to the <c>ISystemScene</c> object.</param>
    /// <param name="pszName">Name of the WaterStream object.</param>
    /// <param name="iStreamObjectID">ID of the object.</param>
    WaterStream( ISystemScene* pSystemScene, pcstr pszName, int iStreamObjectID );

    /// <summary cref="WaterStream::~WaterStream">
    /// dtor
    /// </summary>
    virtual ~WaterStream( void );

    /// <summary cref="WaterStream::ChangeOccurred">
    /// Implementation of the <c>IObserver::ChangeOccurred</c> method.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error
    /// </returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );
    
    /// <summary cref="WaterStream::ChangeOccurred">
    /// Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime/*, std::vector<WaterObject*> WaterObjectList*/ );

    /// <summary cref="WaterStream::GetVelocity">
    /// Implementation of the <c>IMoveObject::GetVelocity</c> method.
    /// </summary>
    /// <returns>Velocity of the object.
    /// </returns>
    /// <seealso cref="IMoveObject::GetVelocity"/>
    virtual const Base::Vector3* GetVelocity( void );

    /// <summary cref="WaterStream::GetPosition">
    /// Implementation of the <c>IGeometryObject::GetPosition</c> method.
    /// </summary>
    /// <returns>Position of the object.
    /// </returns>
    /// <seealso cref="IGeometryObject::GetPosition"/>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="WaterStream::GetMaxVelocity">
    /// Implementation of the <c>IMoveObject::GetMaxVelocity</c> method.
    /// </summary>
    /// <returns>Magnitude of the velocity of the object.
    /// </returns>
    /// <seealso cref="IMoveObject::GetMaxVelocity"/>
    virtual const f32 GetMaxVelocity( void ) { return m_Velocity.Magnitude(); }

protected:

    /// <summary cref="WaterStream::GetDesiredSystemChanges">
    /// Implementation of the <c>ISubject::GetDesiredSystemChanges</c> method.
    /// </summary>
    /// <returns>Returns systems changes requested by this WaterStream
    /// </returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="WaterStream::GetPotentialSystemChanges">
    /// Implementation of the <c>ISubject::GetPotentialSystemChanges</c> method.
    /// </summary>
    /// <returns>Returns systems changes possible for this WaterStream
    /// </returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="WaterStream::GetProperties">
    /// Implementation of the <c>ISystem::GetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Get properties for this WaterStream object.</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="WaterStream::SetProperties">
    /// Implementation of the <c>ISystem::SetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Set properties for this WaterStream object.</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

public:

    /// <summary cref="WaterStream::SetVelocity">
    /// Sets the velocity for the WaterStream object.
    /// </summary>
    /// <param name="velocity">Velocity of the object.</param>
    void WaterStream::SetVelocity(Base::Vector3 velocity);

private:

    /// <summary >
    /// WaterStream system PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_Unused, // Properties are TBD for this object
        Property_Count
    };

    /// <summary >
    /// WaterStream velocity vector
    /// </summary >
    Base::Vector3 m_Velocity; // Velocity vector describing the explosive effect on other entities

    /// <summary >
    /// WaterStream system Property name array
    /// </summary >
    static pcstr sm_kapszPropertyNames[];
    /// <summary >
    /// WaterStream system Property array of default values
    /// </summary >
    static const Properties::Property sm_kaDefaultProperties[]; 

    /// <summary >
    /// WaterStream camera position used to recycle water "comets" from impact with an 
    /// object back to the point of origin
    /// </summary >
    Base::Vector3 m_CameraPosition;

    //Base::Vector3 m_CameraLookAt;
    Base::Quaternion m_CameraOrientation;

    f32 m_WaterStreamLifetime;
    int m_ObjectID;
    Bool m_bUseFireHose;
    Bool m_bSprayWater;
    Bool m_bRepositionWater;
};

