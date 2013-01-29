//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <PagedGeometry.h>
#include <GrassLoader.h>
//system
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
                          "Width", "Height", NULL, NULL,
                          10.0, 10.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MaximumSize ],
                          VALUE1x2(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Width", "Height", NULL, NULL,
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
                          "Densitymap", "Density", NULL, NULL,
                          "", 0.001 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MapBounds ],
                          VALUE1x4(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          0.0, 0.0, 0.0, 0.0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_MaterialName ],
                          VALUE1(Properties::Values::String),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Color ],
                          VALUE1(Properties::Values::String),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Colormap", NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_RenderTechnique ],
                          VALUE2(Properties::Values::Int32,
                                 Properties::Values::Boolean),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Technique", "Fade", NULL, NULL,
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
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid &&
             it->GetFlags() & Properties::Flags::InitOnly )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_MinimumSize ] )
            {
                m_pLayer->setMinimumSize(it->GetFloat32(0), it->GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MaximumSize ] )
            {
                m_pLayer->setMaximumSize(it->GetFloat32(0), it->GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Animation ] )
            {
                m_pLayer->setAnimationEnabled(it->GetBool(0) == True);
                m_pLayer->setSwayDistribution(it->GetFloat32(1));
                m_pLayer->setSwayLength(it->GetFloat32(2));
                m_pLayer->setSwaySpeed(it->GetFloat32(3));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Density ] )
            {
                m_pLayer->setDensityMap(it->GetString(0));
                m_pLayer->setDensity(it->GetFloat32(1));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MapBounds ] )
            {
                m_pLayer->setMapBounds(Forests::TBounds(it->GetFloat32(0), it->GetFloat32(1), it->GetFloat32(2), it->GetFloat32(3)));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MaterialName ] )
            {
                //
                // Load paged geometry layer
                //
                m_pLayer = PSCENE->GetGrassLoader()->addLayer(it->GetString(0));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Color ] )
            {
                m_pLayer->setColorMap(it->GetString(0));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_RenderTechnique ] )
            {
                m_pLayer->setRenderTechnique((Forests::GrassTechnique)it->GetInt32(0), it->GetBool(1) == True );
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

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
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
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        //
        // Set this property to invalid since it's already been read.
        //
        it->ClearFlag( Properties::Flags::Valid );
    }

    return;
}
