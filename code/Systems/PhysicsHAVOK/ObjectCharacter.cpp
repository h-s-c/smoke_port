// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// External
#include <Common\Base\hkBase.h>
#include <Physics\Collide\Shape\hkpShape.h>
#include <Physics\Collide\Shape\Convex\Capsule\hkpCapsuleShape.h>
#include <Physics\Collide\Shape\Misc\Transform\hkpTransformshape.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Dynamics\Phantom\hkpSimpleShapePhantom.h>
#include <Physics\Utilities\CharacterControl\CharacterProxy\hkpCharacterProxy.h>
#include <Physics\Collide\Query\Collector\PointCollector\hkpClosestCdPointCollector.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
// System
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectCharacter.hpp"

// Constants
#define PHAVOKSCENE         reinterpret_cast<HavokPhysicsScene*>(m_pSystemScene)

pcstr HavokCharacterObject::sm_kapszCommonPropertyNames[] =
{
    "CapsuleA", "CapsuleB", "Radius",
};

const Properties::Property HavokCharacterObject::sm_kaCommonDefaultProperties[] =
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

COMPILE_ASSERT( HavokCharacterObject::Property_Count == sizeof HavokCharacterObject::sm_kapszCommonPropertyNames / sizeof HavokCharacterObject::sm_kapszCommonPropertyNames[ 0 ] );

///////////////////////////////////////////////////////////////////////////////
// HavokCharacterObject - Default constructor
HavokCharacterObject::HavokCharacterObject(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : HavokObject( pSystemScene, pszName )
    , m_CharacterProxy( NULL )
    , m_Velocity( Base::Vector3::Zero )
    , m_CapsuleA( Base::Vector3::Zero )
    , m_CapsuleB( Base::Vector3::Zero )
    , m_Radius( 0.0f )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokCharacterObject - Default destructor
HavokCharacterObject::~HavokCharacterObject(
    void
    )
{
    // Free resources
    if( m_CharacterProxy )
    {
        hkpWorld* pWorld = PHAVOKSCENE->GetWorld();

        // Free Havok resources for m_CharacterProxy
        pWorld->lock();

        pWorld->removePhantom( m_CharacterProxy->getShapePhantom() );
        m_CharacterProxy->removeReference();

        pWorld->unlock();
    }
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this object with the given properties
Error
HavokCharacterObject::Initialize(
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
    hkpWorld* pWorld = PHAVOKSCENE->GetWorld();
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
HavokCharacterObject::GetProperties(
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
HavokCharacterObject::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Get system changes this Object is interested in
System::Types::BitMask
HavokCharacterObject::GetDesiredSystemChanges(
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
HavokCharacterObject::Update(
    f32 DeltaTime
    )
{
    //
    // Get the world.
    //
    hkpWorld* pWorld = PHAVOKSCENE->GetWorld();
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
HavokCharacterObject::ChangeOccurred(
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
HavokCharacterObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Geometry::Position;
}

