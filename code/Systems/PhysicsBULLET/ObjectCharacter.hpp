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

#pragma once


class BulletObject;
class BulletPhysicsSyst0em;
class BulletPhysicsScene;
class BulletPhysicsTask;
class btKinematicCharacterController;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>BulletCharacterObject</c> Implementation of the ISystemObject interface.  
///   This is the Character object created objects using Bullet Character Proxy.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class BulletCharacterObject : public BulletObject
{
    friend BulletPhysicsSystem;
    friend BulletPhysicsScene;
    friend BulletPhysicsTask;

protected:

    BulletCharacterObject( ISystemScene* pSystemScene, pcstr pszName );
    ~BulletCharacterObject( void );

    /// <summary cref="BulletCharacterObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the Character object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="BulletCharacterObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the Character object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="BulletCharacterObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the Character object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="BulletCharacterObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the Character object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="BulletCharacterObject::Update">
    /// Called by the task to have the object update itself.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="BulletObject::Update"/>
    virtual void Update( f32 DeltaTime = 0.0f );

    /////////////////////////////////
    /// IObserver overrides
    /////////////////////////////////

    /// <summary cref="BulletCharacterObject::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );


    /// <summary cref="BulletCharacterObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this Object.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

protected:

    btKinematicCharacterController* m_CharacterProxy;

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

