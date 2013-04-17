//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectLight.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


#define POGRESCENEMGR (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRESceneManager())


const pcstr OGREGraphicsObjectLight::sm_kapszLightTypeEnumOptions[] =
{
    "Point", "Spot",
    nullptr
};

pcstr OGREGraphicsObjectLight::sm_kapszPropertyNames[] =
{
    "Type", "Position",
    "Diffuse", "Specular",
    "Direction", "Range", "Attenuation",    // spotlight only
};

const Properties::Property OGREGraphicsObjectLight::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Type ],
                          VALUE1( Properties::Values::Enum ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          LightType_Point ),
    Properties::Property( sm_kapszPropertyNames[ Property_Position ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          Base::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Diffuse ],
                          Properties::Values::Color3,
                          Properties::Flags::Valid,
                          "R", "G", "B", nullptr,
                          Base::Color3::Black ),
    Properties::Property( sm_kapszPropertyNames[ Property_Specular ],
                          Properties::Values::Color3,
                          Properties::Flags::Valid,
                          "R", "G", "B", nullptr,
                          Base::Color3::Black ),
    Properties::Property( sm_kapszPropertyNames[ Property_Direction ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          Base::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Range ],
                          VALUE3( Properties::Values::Angle, Properties::Values::Angle,
                                  Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          Base::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Attenuation ],
                          VALUE1x4( Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          Base::Vector3::Zero ),
};


OGREGraphicsObjectLight::OGREGraphicsObjectLight(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : OGREGraphicsObject( pSystemScene, pszName )
    , m_LightType( LightType_Invalid )
    , m_pLight( nullptr )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Type = OGREGraphicsObject::Type_Light;
}


OGREGraphicsObjectLight::~OGREGraphicsObjectLight(
    void
    )
{
    if ( m_pLight != nullptr )
    {
        m_pNode->detachObject( m_pLight );
        POGRESCENEMGR->destroyLight( m_pLight );
    }
}


Error
OGREGraphicsObjectLight::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err = Errors::Failure;

    ASSERT( !m_bInitialized );

    //
    // Call the base class.
    //
    OGREGraphicsObject::Initialize( Properties );

    //
    // Read in the initialization only properties.
    //
    for (auto & property : Properties)
    {
        if ( property.GetFlags() & Properties::Flags::Valid &&
             property.GetFlags() & Properties::Flags::InitOnly )
        {
            std::string sName = property.GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Type ] )
            {
                //
                // Get the light type.
                //
                m_LightType = static_cast<LightTypes>(property.GetInt32( 0 ));
                ASSERT( m_LightType > LightType_Invalid );
                ASSERT( m_LightType < LightType_Count );

                //
                // Set this property to invalid since it's already been read.
                //
                property.ClearFlag( Properties::Flags::Valid );
            }
        }
    }
    ASSERT( m_LightType != LightType_Invalid );

    //
    // Create the light.
    //
    m_pLight = POGRESCENEMGR->createLight( m_pszName );

    switch ( m_LightType )
    {
    case LightType_Point:
        m_pLight->setType( Ogre::Light::LT_POINT );
        break;

    case LightType_Spot:
        m_pLight->setType( Ogre::Light::LT_SPOTLIGHT );
        break;

    default:
        ASSERT( False );
    };

    if ( m_pLight != nullptr )
    {
        //
        // Set this set as initialized.
        //
        m_bInitialized = True;

        //
        // Set the remaining properties for this object.
        //
        SetProperties( Properties );

        Err = Errors::Success;
    }

    return Err;
}


void
OGREGraphicsObjectLight::GetProperties(
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

    for (auto & property : sm_kaDefaultProperties)
    {
        Properties.push_back( property );
    }

    //
    // Set the enum options string array.
    //
    Properties[ iProperty+Property_Type ].SetEnumOptions( sm_kapszLightTypeEnumOptions );

    //
    // Modify the default values.
    //
    if ( m_pLight != nullptr )
    {
        Properties[ iProperty+Property_Type ].SetValue( 0, m_LightType );

        Ogre::Vector3 OgrePos = m_pLight->getPosition();
        Base::Vector3 Position(OgrePos.x, OgrePos.y, OgrePos.z);
        Properties[ iProperty+Property_Position ].SetValue( Position );

        const Ogre::ColourValue OgreDiffuseColor = m_pLight->getDiffuseColour();
        Base::Color3 DiffuseColor = { OgreDiffuseColor.r, OgreDiffuseColor.g, OgreDiffuseColor.b };
        Properties[ iProperty+Property_Diffuse ].SetValue( DiffuseColor );

        const Ogre::ColourValue OgreSpecularColor = m_pLight->getSpecularColour();
        Base::Color3 SpecularColor = { OgreSpecularColor.r, OgreSpecularColor.g,
                                        OgreSpecularColor.b };
        Properties[ iProperty+Property_Diffuse ].SetValue( SpecularColor );

        if ( m_LightType == LightType_Spot )
        {
            Ogre::Vector3 OgreDir = m_pLight->getDirection();
            Base::Vector3 Direction( OgreDir.x, OgreDir.y, OgreDir.z);
            Properties[ iProperty+Property_Direction ].SetValue( Direction );

            Properties[ iProperty+Property_Range ].SetValue(
                0, static_cast<f32>(m_pLight->getSpotlightInnerAngle().valueRadians())
                );
            Properties[ iProperty+Property_Range ].SetValue(
                1, static_cast<f32>(m_pLight->getSpotlightOuterAngle().valueRadians())
                );
            Properties[ iProperty+Property_Range ].SetValue(
                2, static_cast<f32>(m_pLight->getSpotlightFalloff())
                );

            Properties[ iProperty+Property_Attenuation ].SetValue(
                0, static_cast<f32>(m_pLight->getAttenuationRange())
                );
            Properties[ iProperty+Property_Attenuation ].SetValue(
                1, static_cast<f32>(m_pLight->getAttenuationConstant())
                );
            Properties[ iProperty+Property_Attenuation ].SetValue(
                2, static_cast<f32>(m_pLight->getAttenuationLinear())
                );
            Properties[ iProperty+Property_Attenuation ].SetValue(
                3, static_cast<f32>(m_pLight->getAttenuationQuadric())
                );
        }
    }
}


void
OGREGraphicsObjectLight::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for (auto & property : Properties)
    {
        if ( property.GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = property.GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Position ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setPosition( property.GetVector3().x,
                                           property.GetVector3().y,
                                           property.GetVector3().z );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Diffuse ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setDiffuseColour( property.GetColor3().r,
                                                property.GetColor3().g,
                                                property.GetColor3().b );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Specular ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setSpecularColour( property.GetColor3().r,
                                                 property.GetColor3().g,
                                                 property.GetColor3().b );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Direction ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setDirection( property.GetVector3().x,
                                            property.GetVector3().y,
                                            property.GetVector3().z );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Range ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setSpotlightRange( Ogre::Radian( property.GetFloat32( 0 ) ),
                                                 Ogre::Radian( property.GetFloat32( 1 ) ),
                                                 property.GetFloat32( 2 ) );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Attenuation ] )
            {
                ASSERT( m_pLight != NULL );

                if ( m_pLight != nullptr )
                {
                    m_pLight->setAttenuation( property.GetFloat32( 0 ),
                                              property.GetFloat32( 1 ),
                                              property.GetFloat32( 2 ),
                                              property.GetFloat32( 3 ) );
                }
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            property.ClearFlag( Properties::Flags::Valid );
        }        
    }
}

