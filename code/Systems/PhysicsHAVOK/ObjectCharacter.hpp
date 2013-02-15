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


class HavokObject;
class HavokPhysicsSyst0em;
class HavokPhysicsScene;
class HavokPhysicsTask;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>HavokCharacterObject</c> Implementation of the ISystemObject interface.  
///   This is the Character object created objects using Havok Character Proxy.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class HavokCharacterObject : public HavokObject
{
    friend HavokPhysicsSystem;
    friend HavokPhysicsScene;
    friend HavokPhysicsTask;

protected:

    HavokCharacterObject( ISystemScene* pSystemScene, pcstr pszName );
    ~HavokCharacterObject( void );

    /// <summary cref="HavokCharacterObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the Character object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="HavokCharacterObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the Character object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="HavokCharacterObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the Character object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="HavokCharacterObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the Character object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="HavokCharacterObject::Update">
    /// Called by the task to have the object update itself.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="HavokObject::Update"/>
    virtual void Update( f32 DeltaTime = 0.0f );

	/////////////////////////////////
	/// IObserver overrides
	/////////////////////////////////

    /// <summary cref="HavokCharacterObject::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );


    /// <summary cref="HavokCharacterObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this Object.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

protected:

    hkpCharacterProxy* m_CharacterProxy;

    Base::Vector3      m_Velocity;

    // Properties
    Base::Vector3      m_CapsuleA;
    Base::Vector3      m_CapsuleB;
    f32                m_Radius;

public:

    enum CommonPropertyTypes
    {
        Property_CapsuleA, Property_CapsuleB, Property_Radius,
        Property_Count
    };
    static pcstr                        sm_kapszCommonPropertyNames[];
    static const Properties::Property   sm_kaCommonDefaultProperties[];
};

