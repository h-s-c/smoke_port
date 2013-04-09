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
class BulletPhysicsSystem;
class BulletPhysicsScene;
class BulletPhysicsTask;

// BulletPhysicsObject Implementation of the ISystemObject interface.  
// This is the Physics object created objects using Bullet Physics.  

class BulletPhysicsObject : public BulletObject, public IContactObject, public IIntersectionObject
{
    friend BulletPhysicsSystem;
    friend BulletPhysicsScene;
    friend BulletPhysicsTask;

public:
    void AddContact( const IContactObject::Info& ContactInfo );
    void AddIntersection( const IIntersectionObject::Info& IntersectionInfo );
    inline btRigidBody* GetRigidBody( void ) { return m_pBody; }

protected:

    BulletPhysicsObject( ISystemScene* pSystemScene, pcstr pszType, pcstr pszName,
                        hkpRigidBody* pBody=NULL );
    ~BulletPhysicsObject( void );
    // ISystemObject implementation
    virtual Error Initialize( std::vector<Properties::Property> Properties );
    virtual void GetProperties( Properties::Array& Properties );
    virtual void SetProperties( Properties::Array Properties );
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    // IObserver implementation
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    // IContactObject implementation
    virtual const IContactObject::Info* GetContact( void );
    virtual const Bool IsStatic( void );
    virtual const IIntersectionObject::InfoArray& GetIntersections( void );
    virtual void Update( f32 DeltaTime = 0.0f );


private:
    void CreateBody( btRigidBodyConstructionInfoo& RigidBodyCInfo );


protected:

    Base::Vector3                       m_Offset;

    btRigidBody*                        m_pBody;
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

