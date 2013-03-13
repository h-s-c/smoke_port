//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <OgrePlugin.h>
#include <OgreGLPlugin.h>
#include <OgreParticleFXPlugin.h>
#include <OgreWindowEventUtilities.h>
//system
#include "Systems/GraphicsOGRE/Extras/CgPlugin.hpp"
#include "Systems/GraphicsOGRE/System.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"


//
// Global variables
//
extern void*                   g_hInstance;
extern ManagerInterfaces       g_Managers;

//
// Static member variables
//
pcstr OGREGraphicsSystem::sm_kapszPropertyNames[] =
{
    "ResourceLocation",
    "WindowName",
    "Resolution",
    "ShadowTextureCount",
    "ShadowTextureSize",
    "FullScreen",
    "VerticalSync",
    "FSAntiAliasing",
};

const Properties::Property OGREGraphicsSystem::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_ResourceLocation ],
                          VALUE4( Properties::Values::String,
                                  Properties::Values::String,
                                  Properties::Values::String,
                                  Properties::Values::Boolean ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly |
                            Properties::Flags::Multiple,
                          "Path", "Type", "Group", "Recursive",
                          "", "", "", 0 ),

    Properties::Property( sm_kapszPropertyNames[ Property_WindowName ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Resolution ],
                          VALUE1x2( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1024, 768 ),
    Properties::Property( sm_kapszPropertyNames[ Property_ShadowTextureCount ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1 ),
    Properties::Property( sm_kapszPropertyNames[ Property_ShadowTextureSize ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1024 ),
    Properties::Property( sm_kapszPropertyNames[ Property_FullScreen ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_VerticalSync ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_FSAntiAliasing ],
                          VALUE1x2( Properties::Values::String),
                          Properties::Flags::Valid,
                          "Type", "Quality", NULL, NULL,
                          "0" /* D3DMULTISAMPLE_NONE */,
                          "0" /* Default Quality level offered by driver */ ),
};


OGREGraphicsSystem::OGREGraphicsSystem(
    void
    )
    : ISystem()
    , m_pRoot( NULL )
    , m_pResourceGroupManager( NULL )
    , m_pRenderSystem( NULL )
    , m_pRenderWindow( NULL )
    , m_uShadowTextureCount( 0 )
    , m_uShadowTextureSize( 0 )

{
    ASSERT( Property_Count == (sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ]) );
    ASSERT( Property_Count == (sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ]) );
}


OGREGraphicsSystem::~OGREGraphicsSystem(
    void
    )
{
}


void
OGREGraphicsSystem::windowClosed(
    Ogre::RenderWindow*
    )
{
    ASSERT( pRenderWindow == m_pRenderWindow );
    UNREFERENCED_PARAM( pRenderWindow );
    
    g_Managers.pEnvironment->Runtime().SetStatus( IEnvironment::IRuntime::Status::Quit );
}


pcstr
OGREGraphicsSystem::GetName(
    void
    )
{
    return System::Names::Graphics;
}


System::Type
OGREGraphicsSystem::GetSystemType(
    void
    )
{
    return System::Types::Graphics;
}


Error
OGREGraphicsSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );
    
    // Reroute log, disable cout logging
    m_pLogManager = new Ogre::LogManager();
    m_pLog = Ogre::LogManager::getSingleton().createLog("logs/Ogre.log", true, false, false);    
    
    //
    // Create Ogre's root.
    //
    m_pRoot = new Ogre::Root("", "", "logs/Ogre.log");

    //
    // Get the resource manager.
    //
    m_pResourceGroupManager =  Ogre::ResourceGroupManager::getSingletonPtr();
    
    // Install the gl rendersystem
#ifdef _DEBUG
    m_pRoot->loadPlugin("RenderSystem_GL_d");
#else
    m_pRoot->loadPlugin("RenderSystem_GL");
#endif

    // Install the particle fx plugin
#ifdef _DEBUG
    m_pRoot->loadPlugin("Plugin_ParticleFX_d");
#else
    m_pRoot->loadPlugin("Plugin_ParticleFX");
#endif

    // Create new plugin
    m_pCgPlugin = new Ogre::CgPlugin();
    // Register
    m_pRoot->installPlugin(m_pCgPlugin);

    //
    // Read in the properties required to initialize Ogre.
    //
    char    szWindowName[ 256 ] = "Window";
    u32     Width = 1024;
    u32     Height = 768;
    Bool    bFullScreen = False;
    Bool    bVerticalSync = True;
    std::string     dFSAAType    = "0";  //D3DMULTISAMPLE_NONE;
    std::string     dFSAAQuality = "0";
    std::vector<std::string> resourcegroups;    

    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        //
        // Make sure this property is valid.
        //
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_ResourceLocation ] )
            {
                pcstr pszName = it->GetStringPtr( 0 );
                pcstr pszLocationType = it->GetStringPtr( 1 );
                pcstr pszResourceGroup = it->GetStringPtr( 2 );
                Bool  bRecursive = it->GetBool( 3 ); 
             
                m_pResourceGroupManager->createResourceGroup( pszResourceGroup, true);             
                m_pResourceGroupManager->addResourceLocation( pszName, pszLocationType, pszResourceGroup, (bRecursive == True) );
                
                resourcegroups.push_back(std::string(pszResourceGroup));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_WindowName ] )
            {
                strcpy_s( szWindowName, sizeof szWindowName, it->GetStringPtr( 0 ) );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Resolution ] )
            {
                Width  = static_cast<u32>(it->GetInt32( 0 ));
                Height  = static_cast<u32>(it->GetInt32( 1 ));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_ShadowTextureCount ] )
            {
                m_uShadowTextureCount  = static_cast<u16>(it->GetInt32( 0 ));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_ShadowTextureSize ] )
            {
                m_uShadowTextureSize  = static_cast<u16>(it->GetInt32( 0 ));
            }
            else if ( sName == sm_kapszPropertyNames[ Property_FullScreen ] )
            {
                bFullScreen = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_VerticalSync ] )
            {
                bVerticalSync = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_FSAntiAliasing ] )
            {                
                dFSAAType    = it->GetString( 0 );
                dFSAAQuality = it->GetString( 1 );
            }
            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
    
    //
    // Intialize the render system and render window.
    //
    auto pRenderList = m_pRoot->getAvailableRenderers();
    
    m_pRenderSystem = pRenderList.front();
    m_pRoot->setRenderSystem(m_pRenderSystem);
    m_pRoot->initialise( false );



    // Setup the Full-screen Anti-Aliasing mode
    Ogre::NameValuePairList params;
    params[ "FSAA" ]        = dFSAAType;
    params[ "FSAAQuality" ] = dFSAAQuality;

    // Note: createRenderWindow() is now called directly so that a render winow is created.  The old calling steps
    // yielded a render system with no render window until after plugins load which causes assertions for CreateParticleSystem
    // which requires a render window at the time the billboard renderer loads.
    m_pRenderWindow = m_pRoot->createRenderWindow( szWindowName, Width, Height, bFullScreen == True, &params);
    ASSERT( m_pRenderWindow != NULL );
    
    //make the window accessible by the input system
    size_t hWnd = 0;
    m_pRenderWindow->getCustomAttribute("WINDOW", &hWnd);
    g_Managers.pPlatform->Window().SetWindowHandle( hWnd );
    g_Managers.pPlatform->Window().SetWindowHeight( Height );
    g_Managers.pPlatform->Window().SetWindowWidth( Width );
    
    // listen to the RenderWindow
    Ogre::WindowEventUtilities::addWindowEventListener( m_pRenderWindow, this );

    //
    // Intialize all the resource groups.
    //
    
    // workaround
    for (auto resourcegroup : resourcegroups)
    {
        m_pResourceGroupManager->initialiseResourceGroup( resourcegroup.c_str() );
        m_pResourceGroupManager->loadResourceGroup( resourcegroup.c_str() );
    }
    
    m_pResourceGroupManager->initialiseAllResourceGroups();

    //
    // Set as initialized.
    //
    m_bInitialized = True;

    //
    // Set the remaining properties.
    //
    SetProperties( Properties );

    return Errors::Success;
}


void
OGREGraphicsSystem::GetProperties(
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
    // Modify the default values if initialized.
    //
    if ( m_pRenderWindow != NULL )
    {
        Properties[ iProperty+Property_WindowName ].SetValue( 0, m_pRenderWindow->getName() );

        Properties[ iProperty+Property_Resolution ].SetValue( 0, m_pRenderWindow->getWidth() );
        Properties[ iProperty+Property_Resolution ].SetValue( 1, m_pRenderWindow->getHeight() );

        Properties[ iProperty+Property_FullScreen ].SetValue( 0, m_pRenderWindow->isFullScreen() );
    }
}


void
OGREGraphicsSystem::SetProperties(
    Properties::Array Properties
    )
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

            if ( sName == sm_kapszPropertyNames[ Property_Resolution ] )
            {
                u32 Width  = static_cast<u32>(it->GetInt32( 0 ));
                u32 Height = static_cast<u32>(it->GetInt32( 1 ));

                m_pRenderWindow->resize( Width, Height );
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


ISystemScene*
OGREGraphicsSystem::CreateScene(
    void
    )
{
    return new OGREGraphicsScene( this );
}


Error
OGREGraphicsSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    OGREGraphicsScene* pScene = reinterpret_cast<OGREGraphicsScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

