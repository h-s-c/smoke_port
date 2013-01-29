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
    , m_pMaterialManager( NULL )
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
    // quit listening to the RenderWindow
    Ogre::WindowEventUtilities::removeWindowEventListener( m_pRenderWindow, this );

    m_pResourceGroupManager->shutdownAll();
    
    // Note: it appears that attempting to unload or uninstall the ParticleFX plugin at all with Ogre1.9
    // will cause heap corruption around the guard pages allocated by the NT memory manager.  Luckily it seems
    // like this is not leaking appreciable resources as the app will soon exit.  This should eventually be revisited
    // should a new version of the ParticleFX plugin and/or Ogre become available.
    // m_pRoot->unloadPlugin("Plugin_ParticleFX");
    // m_pRoot->uninstallPlugin("Plugin_ParticleFX");
    
    m_pRoot->shutdown();

    SAFE_DELETE( m_pRoot );
}


void
OGREGraphicsSystem::windowClosed(
    Ogre::RenderWindow* pRenderWindow
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

    //
    // Create Ogre's root.
    //
    m_pRoot = new Ogre::Root( "", "", "Ogre.log" );
    ASSERT( m_pRoot != NULL );

    //
    // Get the resource manager.
    //
    m_pResourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();

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
             
                m_pResourceGroupManager->addResourceLocation(
                    pszName, pszLocationType, pszResourceGroup, (bRecursive == True) );

                m_pResourceGroupManager->loadResourceGroup( pszResourceGroup );
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
    // Command line overrides
    //
    /*{
        USES_CONVERSION;

        WCHAR**             argv;
        int                 argc, iArg;

        argv = CommandLineToArgvW( GetCommandLineW(), &argc);
        for(iArg=0; iArg<argc; iArg++)
        {
            switch (*argv[iArg])
            {
                case '-':
                case '/':
                {
                    if (0==_wcsicmp(argv[iArg]+1, L"windowed" ))    bFullScreen = False;
                }   break;
                
                default:
                {
                }
            }
        }
        
        LocalFree(argv);
    }*/
    
    //
    // Intialize the render system and render window.
    //

#ifdef _DEBUG
    m_pRoot->loadPlugin("RenderSystem_GL_d");
#else
    m_pRoot->loadPlugin("RenderSystem_GL");
#endif

    //Ogre::RenderSystemList *pRenderList;

    auto pRenderList = m_pRoot->getAvailableRenderers();


    m_pRenderSystem = pRenderList.front();
    m_pRoot->setRenderSystem(m_pRenderSystem);
    m_pRoot->initialise( false );

    // Install the particle fx plugin
#ifdef _DEBUG
    m_pRoot->loadPlugin("Plugin_ParticleFX_d");
#else
    m_pRoot->loadPlugin("Plugin_ParticleFX");
#endif

    // Setup the Full-screen Anti-Aliasing mode
    Ogre::NameValuePairList params;
    params[ "FSAA" ]        = dFSAAType;
    params[ "FSAAQuality" ] = dFSAAQuality;

    // Note: createRenderWindow() is now called directly so that a render winow is created.  The old calling steps
    // yielded a render system with no render window until after plugins load which causes assertions for CreateParticleSystem
    // which requires a render window at the time the billboard renderer loads.
    m_pRenderWindow = m_pRoot->createRenderWindow( szWindowName, Width, Height, bFullScreen == True, &params );
    /*m_pRenderWindow =
        m_pRenderSystem->createRenderWindow(
            szWindowName, Width, Height, bFullScreen == True, &params
            );*/
    ASSERT( m_pRenderWindow != NULL );


    // listen to the RenderWindow
    Ogre::WindowEventUtilities::addWindowEventListener( m_pRenderWindow, this );

    //
    // Intialize all the resource groups.
    //
    m_pResourceGroupManager->initialiseAllResourceGroups();

    //
    // Initialize the material manager.
    //

    // Note: Commented because the createRenderWindow() call is now called directly through m_pRoot rather than initialised manually
    // m_pMaterialManager = Ogre::MaterialManager::getSingletonPtr();
    // m_pMaterialManager->initialise();

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

