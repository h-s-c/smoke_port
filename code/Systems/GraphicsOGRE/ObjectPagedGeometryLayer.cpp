#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"

#include <Ogre.h>

#include "Systems/GraphicsOGRE/Extras/PagedGeometry.hpp"
#include "Systems/GraphicsOGRE/Extras/GrassLoader.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectPagedGeometryLayer.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"

#define PSCENE        (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene))
#define POGRESCENEMGR (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRESceneManager())

pcstr OGREGraphicsObjectLayer::sm_kapszPropertyNames[] =
{
    "MinSize", "MaxSize", "Animation", "Density", "MapBounds", "Material", "Color", "Render"
};

const Properties::Property OGREGraphicsObjectLayer::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_MinimumSize ],
                          VALUE1x2( Properties::Values::Float32 ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Width", "Height", nullptr, nullptr,
                          10.0, 10.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MaximumSize ],
                          VALUE1x2(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Width", "Height", nullptr, nullptr,
                          10.0, 10.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_Animation ],
                          VALUE4(Properties::Values::Boolean,
                                 Properties::Values::Float32,
                                 Properties::Values::Float32,
                                 Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Enabled", "SwayDistribution", "Swaylength", "Swayspeed",
                          0, 1.0, 0.0, 0.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_Density ],
                          VALUE2(Properties::Values::String,
                                 Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Densitymap", "Density", nullptr, nullptr,
                          "", 0.001 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MapBounds ],
                          VALUE1x4(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          0.0, 0.0, 0.0, 0.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MaterialName ],
                          VALUE1(Properties::Values::String),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Color ],
                          VALUE1(Properties::Values::String),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Colormap", nullptr, nullptr, nullptr,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_RenderTechnique ],
                          VALUE2(Properties::Values::Int32,
                                 Properties::Values::Boolean),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Technique", "Fade", nullptr, nullptr,
                          "", 0 ),
};


OGREGraphicsObjectLayer::OGREGraphicsObjectLayer(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : OGREGraphicsObject( pSystemScene, pszName )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );
}


OGREGraphicsObjectLayer::~OGREGraphicsObjectLayer(
    void
    )
{
}


Error
OGREGraphicsObjectLayer::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err = Errors::Success;

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

            if ( sName == sm_kapszPropertyNames[ Property_MinimumSize ] )
            {
                m_pLayer->setMinimumSize(property.GetFloat32(0), property.GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MaximumSize ] )
            {
                m_pLayer->setMaximumSize(property.GetFloat32(0), property.GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Animation ] )
            {
                m_pLayer->setAnimationEnabled(property.GetBool(0) == True);
                m_pLayer->setSwayDistribution(property.GetFloat32(1));
                m_pLayer->setSwayLength(property.GetFloat32(2));
                m_pLayer->setSwaySpeed(property.GetFloat32(3));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Density ] )
            {
                m_pLayer->setDensityMap(property.GetString(0));
                m_pLayer->setDensity(property.GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MapBounds ] )
            {
                m_pLayer->setMapBounds(Forests::TBounds(property.GetFloat32(0), property.GetFloat32(1), property.GetFloat32(2), property.GetFloat32(3)));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MaterialName ] )
            {
                //
                // Load paged geometry layer
                //
                m_pLayer = PSCENE->GetGrassLoader()->addLayer(property.GetString(0));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Color ] )
            {
                m_pLayer->setColorMap(property.GetString(0));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_RenderTechnique ] )
            {
                m_pLayer->setRenderTechnique((Forests::GrassTechnique)property.GetInt32(0), property.GetBool(1) == True );
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

    m_pLayer->setFadeTechnique(Forests::FADETECH_ALPHA);

    SetProperties( Properties );

    return Err;
}


void
OGREGraphicsObjectLayer::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty;
    iProperty = static_cast<i32>(Properties.size());

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for (auto & property : sm_kaDefaultProperties)
    {
        Properties.push_back( property );
    }

    return;
}


void
OGREGraphicsObjectLayer::SetProperties(
    Properties::Array Properties
    )
{
    //ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for (auto & property : Properties)
    {
        //
        // Set this property to invalid since it's already been read.
        //
        property.ClearFlag( Properties::Flags::Valid );
    }

    return;
}
