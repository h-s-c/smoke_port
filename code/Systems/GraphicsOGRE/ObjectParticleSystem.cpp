//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <OgreParticleSystem.h>
//standard
#include <mutex>
//system
#include "Systems/GraphicsOGRE/DynamicBuffer.hpp"
#include "Systems/GraphicsOGRE/MovableText.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/Task.hpp"
#include "Systems/GraphicsOGRE/ObjectParticleSystem.hpp"
#include "Systems/GraphicsOGRE/ObjectCamera.hpp"
#include "Systems/GraphicsOGRE/ObjectMesh.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"
//FIXME
#include "Systems/Input/Object.hpp"



#define POGRESCENEMGR (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRESceneManager())
#define PSYSTEM       (reinterpret_cast<OGREGraphicsSystem*>(m_pSystemScene->GetSystem()))

u32 OGREGraphicsObjectParticleSystem::sm_EntityId = 0;

pcstr OGREGraphicsObjectParticleSystem::sm_kapszPropertyNames[] =
{
    "PSystemType",
    "PSystemScripts"
};

const pcstr STR_DEFAULT = "Default";
const pcstr STR_FIREHOSE = "FireHose";

const Properties::Property OGREGraphicsObjectParticleSystem::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_PSystemType ],
    VALUE1( Properties::Values::String ),
    Properties::Flags::Valid | Properties::Flags::InitOnly,
    NULL, NULL, NULL, NULL,
    "" ),

    Properties::Property( sm_kapszPropertyNames[ Property_PSystemScripts ],
    VALUE2(
    Properties::Values::String, Properties::Values::String),
    Properties::Flags::Valid | Properties::Flags::InitOnly,
    NULL, NULL, NULL, NULL,
    "", "" ),
};


OGREGraphicsObjectParticleSystem::OGREGraphicsObjectParticleSystem( ISystemScene* pSystemScene, pcstr pszName )
: OGREGraphicsObject( pSystemScene, pszName ), m_pParticleSystem( NULL ), m_strPSystemName( "" ), m_strPSystemSource( "" ), m_PSystemType( Type_Default )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Type = OGREGraphicsObject::Type_ParticleSystem;
    m_bEmitParticles = False;
}


OGREGraphicsObjectParticleSystem::~OGREGraphicsObjectParticleSystem( void )
{
    m_pNode->detachObject( m_pParticleSystem );
    POGRESCENEMGR->destroyParticleSystem( m_pParticleSystem );
}


Error OGREGraphicsObjectParticleSystem::Initialize( std::vector<Properties::Property> Properties )
{
    ASSERT( !m_bInitialized );

    //
    // Call the base class.
    //
    OGREGraphicsObject::Initialize( Properties );

    //
    // Read in the initialization only properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid &&
            it->GetFlags() & Properties::Flags::InitOnly )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_PSystemType ] )
            {
                std::string strType = it->GetString( 0 );

                if( _stricmp( strType.c_str(), STR_DEFAULT ) == 0 )
                {
                    m_PSystemType = Type_Default;
                }
                else if( _stricmp( strType.c_str(), STR_FIREHOSE ) == 0 )
                {
                    m_PSystemType = Type_FireHose;
                }
                else
                {
                    ASSERTMSG( False, "Invalid particle system property type specified." );
                }
                
            }
            else if ( sName == sm_kapszPropertyNames[ Property_PSystemScripts ] )
            {
                m_strPSystemName = it->GetString( 0 );
                m_strPSystemSource = it->GetString( 1 );

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    //
    // Set the remaining properties for this object.
    //
    SetProperties( Properties );

    if( ( m_strPSystemName != "" ) && ( m_strPSystemSource != "" ) )
    {
        // Note: first arg is unique name followed by the .particle script name
        m_pParticleSystem = POGRESCENEMGR->createParticleSystem( m_strPSystemName, m_strPSystemSource );
        ASSERT( m_pParticleSystem );

        m_pNode->attachObject( m_pParticleSystem );

        m_emissionRate = m_pParticleSystem->getEmitter(0)->getEmissionRate();

        if( m_PSystemType == Type_FireHose )
        {
            // Note: getEmitter() refers to an index of emitters.
            // The fire hose only has one emitter, so index 0 is used specifically here.
            m_pParticleSystem->getEmitter(0)->setEmissionRate( 0 );
        }
    }
    else
    {
        ASSERT( False );
    }

    return Errors::Success;
}

void
OGREGraphicsObjectParticleSystem::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }

    //
    // Modify the default values.
    //
    Properties[ iProperty + Property_PSystemType ].SetValue( 0, "Default" );
    Properties[ iProperty + Property_PSystemScripts ].SetValue( 0, "WaterMaterial" );
    Properties[ iProperty + Property_PSystemScripts ].SetValue( 0, "WaterSource" );
}


void OGREGraphicsObjectParticleSystem::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_PSystemType ] )
            {
                std::string strType = it->GetString( 0 );

                if( _stricmp( strType.c_str(), STR_DEFAULT ) == 0 )
                {
                    m_PSystemType = Type_Default;
                }
                else if( _stricmp( strType.c_str(), STR_FIREHOSE ) == 0 )
                {
                    m_PSystemType = Type_FireHose;
                }
                else
                {
                    ASSERTMSG( False, "Invalid particle system property type specified." );
                }

            }
            else if ( sName == sm_kapszPropertyNames[ Property_PSystemScripts  ] )
            {
                m_strPSystemName = it->GetString( 0 );
                m_strPSystemSource = it->GetString( 1 );
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}


System::Types::BitMask OGREGraphicsObjectParticleSystem::GetDesiredSystemChanges( void )
{
    return System::Changes::Input::Firehose | System::Changes::Geometry::Position | System::Changes::Geometry::Orientation | System::Changes::Geometry::Scale;
}


Error OGREGraphicsObjectParticleSystem::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    ASSERT( m_bInitialized );

    if ( !m_pNode )
    {
        // We may get updates before the object is properly initialized which is OK.

        // StaticGeom and InstancedGeom objects are abstract groupings and 
        // are not globally attached to any scene node
        return Errors::Success;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if( m_PSystemType == Type_FireHose )
    {
        if( ChangeType & System::Changes::Input::Firehose )
        {
            InputObject* pInputObj = dynamic_cast<InputObject*>(pSubject);
            if( pInputObj )
            {
                if( m_bEmitParticles )
                {
                    // Turn it off
                    m_pParticleSystem->getEmitter(0)->setEmissionRate( 0 ); // Uses emitter index 0 - specific to the fire hose which has only 1
                    m_bEmitParticles = false;
                }
                else
                {
                    // Turn it on
                    m_pParticleSystem->getEmitter(0)->setEmissionRate( m_emissionRate ); // Uses emitter index 0 - specific to the fire hose which has only 1
                    m_bEmitParticles = true;
                }
            }
        }
    }

    if ( ChangeType & System::Changes::Geometry::Position )
    {
        if( m_PSystemType == Type_Default )
        {
            IGeometryObject* pGeometryObj = dynamic_cast<IGeometryObject*>(pSubject);
            if( pGeometryObj )
            {
                m_Position = *pGeometryObj->GetPosition();
            }
        }
        else if( m_PSystemType == Type_FireHose )
        {
            OGREGraphicsObjectCamera* pOgreCamObj = dynamic_cast<OGREGraphicsObjectCamera*>(pSubject);
            if( pOgreCamObj )
            {
                m_Position = *pOgreCamObj->GetPosition();

                m_Position.y -= 5.0f;

                m_pNode->setPosition( m_Position.x, m_Position.y, m_Position.z );                
            }
        }

        // Force the bounding box to update
        m_pNode->_updateBounds();
    }

    if ( ChangeType & System::Changes::Geometry::Orientation)
    {
        if( m_PSystemType == Type_Default )
        {
            IGeometryObject* pGeometryObj = dynamic_cast<IGeometryObject*>(pSubject);
            if( pGeometryObj )
            {
                m_Orientation = *pGeometryObj->GetOrientation();
                m_pNode->setOrientation( m_Orientation.w, m_Orientation.x, m_Orientation.y, m_Orientation.z );
            }
        }
        else if( m_PSystemType == Type_FireHose )
        {
            OGREGraphicsObjectCamera* pOgreCamObj = dynamic_cast<OGREGraphicsObjectCamera*>(pSubject);
            if( pOgreCamObj )
            {
                m_Orientation = *pOgreCamObj->GetOrientation();

                m_pNode->setOrientation( m_Orientation.w, m_Orientation.x, m_Orientation.y, m_Orientation.z );
            }
        }

        // Force the bounding box to update
        m_pNode->_updateBounds();
    }

    if ( ChangeType & System::Changes::Geometry::Scale )
    {
        IGeometryObject* pGeometryObj = dynamic_cast<IGeometryObject*>(pSubject);
        if( pGeometryObj )
        {
            m_Scale = *pGeometryObj->GetScale();
            m_pNode->setScale( m_Scale.x, m_Scale.y, m_Scale.z );
        }

        // Force the bounding box to update
        m_pNode->_updateBounds();
    }

    return Errors::Success;
}


System::Changes::BitMask OGREGraphicsObjectParticleSystem::GetPotentialSystemChanges( void )
{
    return System::Changes::None;
}


void OGREGraphicsObjectParticleSystem::Update( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


const Math::Vector3* OGREGraphicsObjectParticleSystem::GetPosition( void )
{   
    return &m_Position;
}


const Math::Quaternion* OGREGraphicsObjectParticleSystem::GetOrientation( void )
{   
    return &m_Orientation;
}


const Math::Vector3* OGREGraphicsObjectParticleSystem::GetScale( void )
{
    return &m_Scale;
}
