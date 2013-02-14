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

#include <windows.h> // For CRITICAL_SECTION

class HavokObject;
class HavokPhysicsSystem;
class HavokPhysicsScene;
class HavokPhysicsTask;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>HavokPhysicsObject</c> Implementation of the ISystemObject interface.  
///   This is the Physics object created objects using Havok Physics.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class HavokPhysicsObject : public HavokObject, public IContactObject, public IIntersectionObject
{
    friend HavokPhysicsSystem;
    friend HavokPhysicsScene;
    friend HavokPhysicsTask;

public:

    /// <summary cref="HavokPhysicsObject::AddContact">
    ///   Adds a new contact (this info might be interesting to other systems)
    /// </summary>
    /// <param name="ContactInfo">Info about the new contact</param>
    void AddContact( const IContactObject::Info& ContactInfo );

    /// <summary cref="HavokPhysicsObject::AddIntersection">
    ///   Adds a new intersection (this info might be interesting to other systems)
    /// </summary>
    /// <param name="IntersectionInfo">Info about the new intersection</param>
    void AddIntersection( const IIntersectionObject::Info& IntersectionInfo );
    
    /// <summary cref="HavokPhysicsObject::GetRigidBody">
    ///   Returns the hkpRigidBody for this Object
    /// </summary>
    /// <returns>hkpRigidBody* - This Objects rigid body</returns>
    inline hkpRigidBody* GetRigidBody( void ) { return m_pBody; }

protected:

    HavokPhysicsObject( ISystemScene* pSystemScene, pcstr pszType, pcstr pszName,
                        hkpRigidBody* pBody=NULL );
    ~HavokPhysicsObject( void );

    /////////////////////////////////
    /// ISystemObject implementation
    /////////////////////////////////

    /// <summary cref="HavokPhysicsObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the Physics object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="HavokPhysicsObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the Physics object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="HavokPhysicsObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the Physics object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="HavokPhysicsObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the Physics object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /////////////////////////////////
    /// IObserver implementation
    /////////////////////////////////

    /// <summary cref="HavokPhysicsObject::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="HavokPhysicsObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this Object.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /////////////////////////////////
    /// IContactObject implementation
    /////////////////////////////////

    /// <summary cref="IContactObject::GetContact">
    ///   Implementation of the IContactObject GetContact function.
    /// </summary>
    virtual const IContactObject::Info* GetContact( void );

    /// <summary cref="IContactObject::IsStatic">
    ///   Implementation of the IContactObject IsStatic function.
    /// </summary>
    virtual const Bool IsStatic( void );

    /// <summary cref="IIntersectionObject::GetIntersections">
    ///   Implementation of the IIntersectionObject GetIntersections function.
    /// </summary>
    virtual const IIntersectionObject::InfoArray& GetIntersections( void );

    /// <summary cref="HavokCharacterObject::Update">
    /// Called by the task to have the object update itself.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="HavokObject::Update"/>
    virtual void Update( f32 DeltaTime = 0.0f );


private:

    /// <summary cref="HavokPhysicsObject::AddIntersection">
    ///   Creates a new rigid body based on RigidBodyCInfo and adds it to the Havok world
    /// </summary>
    /// <param name="RigidBodyCInfo">Descriptor of rigid body to create.</param>
    void CreateBody( hkpRigidBodyCinfo& RigidBodyCInfo );


protected:

    Base::Vector3                       m_Offset;

    hkpRigidBody*                       m_pBody;
    Bool                                m_bStatic;

    i32                                 m_MaterialId;
    f32                                 m_Mass;

    Base::Vector3                       m_LinearVelocity;

    i32                                 m_Quality;

    void*                               m_pShapeData1;
    void*                               m_pShapeData2;

    IContactObject::Info                m_ContactInfo;
    IIntersectionObject::InfoArray      m_aIntersectionInfo;

private:

    static pcstr                        sm_kapszTypeNames[];

    enum Types
    {
        Type_Box, Type_Sphere, Type_ConvexHull, Type_Mesh, Type_Space, Type_Dynamic,
    };
    Types                               m_Type;

    enum CommonPropertyTypes
    {
        Property_Mass, Property_Static, Property_Material, Property_LinearVelocity, Property_Quality,
        Property_Count
    };
    static pcstr                        sm_kapszCommonPropertyNames[];
    static const Properties::Property   sm_kaCommonDefaultProperties[];

    enum BoxPropertyTypes
    {
        BoxProperty_Lengths,
        BoxProperty_Count
    };
    static pcstr                        sm_kapszBoxPropertyNames[];
    static const Properties::Property   sm_kaBoxDefaultProperties[];

    enum SpherePropertyTypes
    {
        SphereProperty_Radii,
        SphereProperty_Count
    };

    static pcstr                        sm_kapszSpherePropertyNames[];
    static const Properties::Property   sm_kaSphereDefaultProperties[];
};

