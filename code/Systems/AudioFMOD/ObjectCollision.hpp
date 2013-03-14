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

//internal
#include "Systems/AudioFMOD/Object.hpp"

// Impact figure to minimize collision noises when smaller impacts occur
#define IMPACT_SPEED_SOUND 1500.0f

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>FMODObjectCollision</c> Implementation of collision sounds
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class FMODObjectCollision : public FMODObject
{
public:
    FMODObjectCollision( ISystemScene* pSystemScene );
    virtual ~FMODObjectCollision( void );

protected:
    /// <summary cref="FMODObjectCollision::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function for collision sounds.
    /// </summary>
    /// <param name="Properties">Initializes the sound object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="FMODObjectCollision::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the sound object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="FMODObjectCollision::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the sound object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="FMODObjectCollision::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the sound object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="FMODObjectCollision::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this sound.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="FMODObjectCollision::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="FMODObjectCollision::UpdateCollisionPosition">
    ///   Updates of the collision
    /// </summary>
    /// <param name="CurrPos">Position of the source to update to</param>
    /// <returns>Error.</returns>
    Error UpdateCollisionPosition( Base::Vector3* CurrPos );

    /// <summary >
    /// FMODObjectCollision PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_CollisionSF, Property_CollisionVolume, Property_CollisionPriority,
        Property_Count
    };
    
    /// <summary >
    /// FMODObjectCollision Property name array
    /// </summary >
    static pcstr                        sm_kapszPropertyNames[];
    
    /// <summary >
    /// FMODObjectCollision Property array of default values
    /// </summary >
    static const Properties::Property   sm_kaDefaultProperties[];

private:
    // Properties
    f32             f_CollisionVolume;
    u32             i_CollisionPriority;
    char            CollisionSFName[FILENAME_MAX];

    // Collision sound data and sound position holders
    FMOD::Sound*    CollisionBuffer;
    FMOD::Channel*  CollisionSource;
};

