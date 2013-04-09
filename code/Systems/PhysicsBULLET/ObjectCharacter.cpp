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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/PhysicsBULLET/Scene.hpp"
#include "Systems/PhysicsBULLET/Object.hpp"
#include "Systems/PhysicsBULLET/ObjectCharacter.hpp"

pcstr BulletCharacterObject::sm_kapszCommonPropertyNames[] =
{
    "CapsuleA", "CapsuleB", "Radius",
};

const Properties::Property BulletCharacterObject::sm_kaCommonDefaultProperties[] =
{
    Properties::Property( sm_kapszCommonPropertyNames[ Property_CapsuleA ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0.0f ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_CapsuleB ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0.0f ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_Radius ],
                          Properties::Values::Float32,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0.0f ),
};

COMPILE_ASSERT( BulletCharacterObject::Property_Count == sizeof BulletCharacterObject::sm_kapszCommonPropertyNames / sizeof BulletCharacterObject::sm_kapszCommonPropertyNames[ 0 ] );

///////////////////////////////////////////////////////////////////////////////
// BulletCharacterObject - Default constructor
BulletCharacterObject::BulletCharacterObject(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : BulletObject( pSystemScene, pszName )
    , m_CharacterProxy( NULL )
    , m_Velocity( Base::Vector3::Zero )
    , m_CapsuleA( Base::Vector3::Zero )
    , m_CapsuleB( Base::Vector3::Zero )
    , m_Radius( 0.0f )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~BulletCharacterObject - Default destructor
BulletCharacterObject::~BulletCharacterObject(
    void
    )
{
    // Free resources
    if( m_CharacterProxy )
    {
        hkpWorld* pWorld = reinterpret_cast<BulletPhysicsScene*>(m_pSystemScene)->GetWorld();

        // Free Bullet resources for m_CharacterProxy
        pWorld->lock();

        pWorld->removePhantom( m_CharacterProxy->getShapePhantom() );
        m_CharacterProxy->removeReference();

        pWorld->unlock();
    }
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this object with the given properties
Error
BulletCharacterObject::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Read in the properties.
    //
    for( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if( sName == sm_kapszCommonPropertyNames[ Property_CapsuleA ] )
            {
                m_CapsuleA = it->GetVector3();
                it->ClearFlag( Properties::Flags::Valid );
            }
            else if( sName == sm_kapszCommonPropertyNames[ Property_CapsuleB ] )
            {
                m_CapsuleB = it->GetVector3();
                it->ClearFlag( Properties::Flags::Valid );
            }
            else if( sName == sm_kapszCommonPropertyNames[ Property_Radius ] )
            {
                m_Radius = it->GetFloat32( 0 );
                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    //
    // Get the world.
    //
    hkpWorld* pWorld = reinterpret_cast<BulletPhysicsScene*>(m_pSystemScene)->GetWorld();
    ASSERT( pWorld != NULL );

    pWorld->lock();

    //
    // Create a capsule to represent the character
    //
    hkVector4 VertexA( m_CapsuleA.x, m_CapsuleA.y, m_CapsuleA.z );
    hkVector4 VertexB( m_CapsuleB.x, m_CapsuleB.y, m_CapsuleB.z );

    hkpShape* Capsule = new hkpCapsuleShape( VertexA, VertexB, m_Radius );

    //
    // Construct a Shape Phantom
    //
    hkpShapePhantom* Phantom = new hkpSimpleShapePhantom( Capsule, hkTransform::getIdentity() );

    //
    // Add the phantom to the world
    //
    pWorld->addPhantom( Phantom );

    //
    // Construct a character proxy
    //
    hkpCharacterProxyCinfo Cinfo;
    Cinfo.m_position.set( m_Position.x, m_Position.y, m_Position.z );
    Cinfo.m_staticFriction = 0.0f;
    Cinfo.m_dynamicFriction = 1.0f;
    Cinfo.m_up.setNeg4( pWorld->getGravity() );
    Cinfo.m_up.normalize3();    

    Cinfo.m_shapePhantom = Phantom;
    m_CharacterProxy = new hkpCharacterProxy( Cinfo );

    Capsule->removeReference();
    Phantom->removeReference();

    pWorld->unlock();
        
    //
    // Set this as initialized.
    //
    m_bInitialized = True;

    //
    // Set the properties for this object.
    //
    SetProperties( Properties );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Get the properties for this Object
void
BulletCharacterObject::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add the common properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaCommonDefaultProperties[ i ] );
    }
}


///////////////////////////////////////////////////////////////////////////////
// Properties - Set the properties for this Object
void
BulletCharacterObject::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Get system changes this Object is interested in
System::Types::BitMask
BulletCharacterObject::GetDesiredSystemChanges(
    void
    )
{
    return ( System::Changes::Geometry::All
           | System::Changes::Graphics::AllMesh
           | System::Changes::AI::Velocity );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update this Object (should be called every frame)
void
BulletCharacterObject::Update(
    f32 DeltaTime
    )
{
    //
    // Get the world.
    //
    hkpWorld* pWorld = reinterpret_cast<BulletPhysicsScene*>(m_pSystemScene)->GetWorld();
    ASSERT( pWorld != NULL );

    //
    // Update the character proxy
    //
    hkVector4 Velocity( m_Velocity.x, m_Velocity.y, m_Velocity.z );

    //
    // If this character isn't supported, apply gravity
    //
    static const hkVector4 down( 0.0f, -1.0f, 0.0f, 0.0f );
    hkpSurfaceInfo ground;
    m_CharacterProxy->checkSupport( down, ground );

    if( ground.m_supportedState != hkpSurfaceInfo::SUPPORTED )
    {
        Velocity.add4( pWorld->getGravity() );
    }

    //
    // Set the new linear velocity
    //
    m_CharacterProxy->setLinearVelocity( Velocity );

    //
    // Integrate (update) character proxity
    //
    hkStepInfo StepInfo( hkTime(0.0f), hkTime( DeltaTime ) );
    m_CharacterProxy->integrate( StepInfo, pWorld->getGravity() );

    //
    // Update the position
    //
    const hkVector4& Position = m_CharacterProxy->getPosition();
    Position.store3( m_Position );

    PostChanges( System::Changes::Geometry::Position );
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Give this Object a change to process this system change
Error
BulletCharacterObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    // Update this objects position
    if ( ChangeType & System::Changes::Geometry::Position )
    {
        m_Position = *dynamic_cast<IGeometryObject*>(pSubject)->GetPosition();

        if ( m_CharacterProxy != NULL )
        {
            // Modify the phantom's position.
            hkVector4 Position( m_Position.x, m_Position.y, m_Position.z );
            m_CharacterProxy->setPosition( Position );
        }
    }

    // Update this objects orientation
    if ( ChangeType & System::Changes::Geometry::Orientation )
    {
        m_Orientation = *dynamic_cast<IGeometryObject*>(pSubject)->GetOrientation();

        if ( m_CharacterProxy != NULL )
        {
            // Modify the phantom's orientation.
            // ...
        }
    }

    // Update this objects velocity
    if ( ChangeType & System::Changes::AI::Velocity )
    {
        // Store the new velocity
        m_Velocity = *dynamic_cast<IMoveObject*>(pSubject)->GetVelocity();
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Get all system change possible for this Object
System::Changes::BitMask
BulletCharacterObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Geometry::Position;
}

