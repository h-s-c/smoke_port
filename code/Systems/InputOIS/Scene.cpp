// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// interface
#include "Interfaces/Interface.hpp"
// Standard Library
#include <iostream>
#include <sstream>
// System
#include "Systems/InputOIS/System.hpp"
#include "Systems/InputOIS/Scene.hpp"
#include "Systems/InputOIS/Object.hpp"
#include "Systems/InputOIS/Task.hpp"


#define SAFE_RELEASE(x)     if (x) { (x)->Release();        (x) = NULL; }   //!< Safe D3D-style release


extern ManagerInterfaces   g_Managers;


InputScene::InputScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )
    , m_pInputTask( NULL )
{
    m_pInputSystem = (InputSystem*)pSystem;
}

InputScene::~InputScene(
    void
    )
{
    //
    // Free all the remaining objects.
    //
    for( std::list<InputObject*>::iterator it=m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();

    //
    // Delete the task object
    //
    SAFE_DELETE( m_pInputTask );
}


System::Type
InputScene::GetSystemType(
    void
    )
{
    return System::Types::Input;
}


Error
InputScene::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    m_pInputTask = new InputTask( this );
    ASSERT( m_pInputTask != NULL );
    if( m_pInputTask == NULL )
    {
        return Errors::Failure;
    }

    m_nRotateLeftRight = 0;
    m_nRotateUpDown    = 0;
    m_nMoveLeftRight   = 0;
    m_nMoveForwardBack = 0;
    m_nMoveUpDown      = 0;
    m_nOldForward      = 0;
    m_nOldBack         = 0;
    m_nOldLeft         = 0;
    m_nOldRight        = 0;
    m_nOldUp           = 0;
    m_nOldDown         = 0;
    m_MouseMoveEnabled = false;
    
    size_t windowHnd = g_Managers.pPlatform->Window().GetWindowHandle();
    std::ostringstream windowHndStr;
    OIS::ParamList pl;

    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    #if defined PLATFORM_WINDOWS
       pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
       pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
       pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
       pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    #elif defined PLATFORM_LINUX
       pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
       pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    #endif

    m_InputManager = OIS::InputManager::createInputSystem( pl);
    m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject(OIS::OISMouse, true));
    m_Mouse->setEventCallback( this );
    m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, true));
    m_Keyboard->setEventCallback( this );
    
    uint32_t height= g_Managers.pPlatform->Window().GetWindowHeight();
    uint32_t width= g_Managers.pPlatform->Window().GetWindowWidth();
    const OIS::MouseState &ms = m_Mouse->getMouseState();
    ms.height = height;
    ms.width = width;

    m_bInitialized = True;

    return Errors::Success;
}


void
InputScene::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


void
InputScene::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
}

    
pcstr*
InputScene::GetObjectTypes(
    void
    )
{
    return NULL;
}


ISystemObject*
InputScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    ASSERT( m_bInitialized );

    //
    // Create the object and add it to the object list.
    //
    InputObject* pObject = NULL;
    if( pszType && pszName ) 
    {
        pObject = new InputObject( this, pszType, pszName);
    } 
    else 
    {
        pObject = new InputObject ( this );
    }

    ASSERT( pObject != NULL );
    if( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    return pObject;
}


Error
InputScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a InputObject so that the correct destructor will be called.
    //
    InputObject* pObject = reinterpret_cast<InputObject*>(pSystemObject);

    //
    // Remove the object from the list and delete it.
    //
    m_Objects.remove( pObject );

    //SAFE_DELETE( pSystemObject );

    return Errors::Success;
}


ISystemTask*
InputScene::GetSystemTask(
    void
    )
{
    return m_pInputTask;
}


System::Changes::BitMask
InputScene::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Graphics::GUI;
}


const void*
InputScene::GetSystemChangeData(
    System::Change SystemChange
    )
{
    UNREFERENCED_PARAM( SystemChange );

    return NULL;
}

void InputScene::InputUpdate(f32 DeltaTime)
{
    // Cap DeltaTime
    DeltaTime = Base::Min( 1.0, DeltaTime );
    
    if( m_Mouse ) 
    {
        m_Mouse->capture();
        if( !m_Mouse->buffered() )
        {
            handleNonBufferedMouse();
        }
    }
 
    if( m_Keyboard ) {
        m_Keyboard->capture();
        if( !m_Keyboard->buffered() )
        {
            handleNonBufferedKeys();
        }
    }

    bool bPositionModified = false;
    bool bOrientationModified = false;
    static float       xSensitivity = 1.25f;
    static float       ySensitivity = 2.5f;
    u32                mModified = 0;
    f32                total_yaw = 0.0f;
    f32                total_pitch = 0.0f;
    
    if( m_nRotateLeftRight || m_nRotateUpDown )
    {
        total_yaw   -= Base::Angle::Deg2Rad( m_nRotateLeftRight * DeltaTime * ySensitivity );
        total_pitch -= Base::Angle::Deg2Rad( m_nRotateUpDown * DeltaTime * xSensitivity );
        bOrientationModified = true;
    }
    
    Base::Vector3 Direction( 0.0f, 0.0f, 0.0f );
    if (m_nMoveLeftRight || m_nMoveForwardBack || m_nMoveUpDown) {
        // When paused DeltaTime is 0 which prevents camera movement on paused scene.  
        // If paused, set DeltaTime to some constant to allow camera movement around the scene.
        if( bScenePaused )
        {
            DeltaTime = 0.02f;
        }
        Direction.x += m_nMoveLeftRight * DeltaTime;
        Direction.z += m_nMoveForwardBack * DeltaTime;
        Direction.y += m_nMoveUpDown * DeltaTime;
        bPositionModified = true;
    }

    // Cycle through all of our objects and apply the changes. Also post our change notifications to the CCM.
    std::list<InputObject*>& Objects = m_Objects;
    if( bPositionModified || bOrientationModified || bUseFireHose )
    {
        for( std::list<InputObject*>::iterator it=Objects.begin(); it != Objects.end(); it++ )
        {
            InputObject* pObject = *it;

            if( bUseFireHose )
            {
                mModified |= System::Changes::Input::Firehose;
            }
            if( pObject->m_Type == pObject->Type_GUI ) 
            {
                if( nToggleOverlay != 0)
                {
                    if( pObject->m_nFunctionKey == nToggleOverlay )
                    {
                        pObject->PostChanges(System::Changes::Graphics::GUI);
                    }
                    nToggleOverlay = 0;
                }
            }
            if( bPositionModified ) 
            {
                pObject->m_Position = Direction;

                mModified |= System::Changes::Geometry::Position;
            }

            if( bOrientationModified ) 
            {            
                // NOTE: there's nothing special about the number 0.2f.  It was determined by trial
                // and error in order to smooth out the movement
                total_pitch = Base::Min( total_pitch, 0.2f );
                total_pitch = Base::Max( total_pitch, -0.2f );
                total_yaw = Base::Min( total_yaw, 0.2f );
                total_yaw = Base::Max( total_yaw, -0.2f );

                pObject->m_Orientation.x = total_yaw;
                pObject->m_Orientation.y = total_pitch;
                pObject->m_Orientation.z = 0.0f;
                pObject->m_Orientation.w = 0.0f;

                mModified |= System::Changes::Geometry::Orientation;
            }
            pObject->PostChanges( mModified );
        }
    }
    return;
}

void InputScene::handleNonBufferedKeys()
{
    if( m_Keyboard->isKeyDown( OIS::KC_ESCAPE ) || m_Keyboard->isKeyDown( OIS::KC_Q ) )
    {
        g_Managers.pEnvironment->Runtime().SetStatus( IEnvironment::IRuntime::Status::Quit );
    }
}

void InputScene::handleNonBufferedMouse()
{
    //Just dump the current mouse state
    const OIS::MouseState &ms = m_Mouse->getMouseState();
    std::cout << "\nUnbuffered Mouse: Abs(" << ms.X.abs << " " << ms.Y.abs << " " << ms.Z.abs
        << ") B: " << ms.buttons << " Rel(" << ms.X.rel << " " << ms.Y.rel << " " << ms.Z.rel << ")";
}

bool 
InputScene::keyPressed( const OIS::KeyEvent &arg ) 
{      
    // Get any modifiers.
    int mModifiers = 0;
    if ( m_Keyboard->isModifierDown( OIS::Keyboard::Modifier::Alt ) )
        mModifiers |= OIS::Keyboard::Modifier::Alt;
    if ( m_Keyboard->isModifierDown( OIS::Keyboard::Modifier::Ctrl ) )
        mModifiers |= OIS::Keyboard::Modifier::Ctrl;
    if ( m_Keyboard->isModifierDown( OIS::Keyboard::Modifier::Shift ) )
        mModifiers |= OIS::Keyboard::Modifier::Shift;
        
    if( arg.key == OIS::KC_P  && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        i32 polygonMode = g_Managers.pEnvironment->Variables().GetAsInt( "Graphics::PolygonMode" ) + 1;
        char t[5];
        sprintf_s( t, 5, "%d", polygonMode );
        g_Managers.pEnvironment->Variables().SetValue( "Graphics::PolygonMode", t );
    }
    if( arg.key == OIS::KC_SPACE )
    {
        bUseFireHose = True;
    }
    if( arg.key == OIS::KC_N  && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        pcstr pszValue = "False";
        Bool bShowNormals = g_Managers.pEnvironment->Variables().GetAsBool( "Graphics::ShowNormals" );
        bShowNormals = !bShowNormals;
        if( bShowNormals ) 
        {
            pszValue = "True";
        }
        g_Managers.pEnvironment->Variables().SetValue( "Graphics::ShowNormals", pszValue );
    }
    if( arg.key == OIS::KC_T  && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        pcstr pszValue = "False";
        Bool bShowTangents = g_Managers.pEnvironment->Variables().GetAsBool( "Graphics::ShowTangents" );
        bShowTangents = !bShowTangents;
        if( bShowTangents ) 
        {
            pszValue = "True";
        }
        g_Managers.pEnvironment->Variables().SetValue( "Graphics::ShowTangents", pszValue );
    }
    if( arg.key == OIS::KC_B  && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        pcstr pszValue = "False";
        Bool bShowBoundingBox = g_Managers.pEnvironment->Variables().GetAsBool( "Graphics::ShowBoundingBox" );
        bShowBoundingBox = !bShowBoundingBox;
        if( bShowBoundingBox ) 
        {
            pszValue = "True";
        }
        g_Managers.pEnvironment->Variables().SetValue( "Graphics::ShowBoundingBox", pszValue );
    }
    if( arg.key == OIS::KC_O  && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        pcstr pszValue = "False";
        Bool bShowCaptions = g_Managers.pEnvironment->Variables().GetAsBool( "Graphics::ShowCaptions" );
        bShowCaptions = !bShowCaptions;
        if( bShowCaptions ) 
        {
            pszValue = "True";
        }
        g_Managers.pEnvironment->Variables().SetValue( "Graphics::ShowCaptions", pszValue );
    }
    if( arg.key == OIS::KC_4 && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        if(g_Managers.pTask != NULL )
        {
            //g_Managers.pTask->SetNumberOfThreads( 4 );
        }
    }
    if( arg.key == OIS::KC_2 && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        if(g_Managers.pTask != NULL )
        {
            //g_Managers.pTask->SetNumberOfThreads( 2 );
        }
    }
    if( arg.key == OIS::KC_1 && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        if(g_Managers.pTask != NULL )
        {
            //g_Managers.pTask->SetNumberOfThreads( 1 );
        }
    }
    if( arg.key == OIS::KC_X && mModifiers == OIS::Keyboard::Modifier::Ctrl )
    {
        if(g_Managers.pTask != NULL )
        {
            //g_Managers.pTask->SetNumberOfThreads( 0 );
        }
    }
    const float        fMovementSpeed = 800.0f;
    if( arg.key == OIS::KC_W)
    {
        m_nOldForward       = -fMovementSpeed;
        m_nMoveForwardBack += m_nOldForward;
    }
    if( arg.key == OIS::KC_S)
    {
        m_nOldBack          = fMovementSpeed;
        m_nMoveForwardBack += m_nOldBack;
    } 
    if( arg.key == OIS::KC_A)
    {
        m_nOldLeft        = -fMovementSpeed;
        m_nMoveLeftRight += m_nOldLeft;
    } 
    if( arg.key == OIS::KC_D)
    {
        m_nOldRight       = fMovementSpeed;
        m_nMoveLeftRight += m_nOldRight;
    } 
    if( arg.key == OIS::KC_R)
    {
        m_nOldUp       = fMovementSpeed;
        m_nMoveUpDown += m_nOldUp;
    } 
    if( arg.key == OIS::KC_F)
    {
        m_nOldDown     = -fMovementSpeed;
        m_nMoveUpDown += m_nOldDown;
    }

    std::cout << " KeyPressed {" << arg.key
        << ", " << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key)
        << "} || Character (" << (char)arg.text << ")" << std::endl;
    return true;
}
bool 
InputScene::keyReleased( const OIS::KeyEvent &arg ) 
{
    if( arg.key == OIS::KC_ESCAPE || arg.key == OIS::KC_Q )
    {
        g_Managers.pEnvironment->Runtime().SetStatus( IEnvironment::IRuntime::Status::Quit );
    }
    if( arg.key == OIS::KC_SPACE )
    {
        bUseFireHose = False;
    }
    if( arg.key == OIS::KC_W)
    {
        m_nMoveForwardBack -= m_nOldForward;
    }
    if( arg.key == OIS::KC_S)
    {
        m_nMoveForwardBack -= m_nOldBack;
    }
    if( arg.key == OIS::KC_A)
    {
        m_nMoveLeftRight -= m_nOldLeft;
    }
    if( arg.key == OIS::KC_D)
    {
        m_nMoveLeftRight -= m_nOldRight;
    }
    if( arg.key == OIS::KC_R)
    {
        m_nMoveUpDown -= m_nOldUp;
    }
    if( arg.key == OIS::KC_F)
    {
        m_nMoveUpDown -= m_nOldDown;
    }
    if( arg.key == OIS::KC_PAUSE)
    {
        switch( g_Managers.pEnvironment->Runtime().GetStatus() )
        {
            case IEnvironment::IRuntime::Status::Running:
                g_Managers.pEnvironment->Runtime().SetStatus( IEnvironment::IRuntime::Status::Paused );
                bScenePaused = true;
                break;
            case IEnvironment::IRuntime::Status::Paused:
                g_Managers.pEnvironment->Runtime().SetStatus( IEnvironment::IRuntime::Status::Running );
                bScenePaused = false;
                break;
            default:
                break;
        }
    }  
    if( arg.key == OIS::KC_F1)
    {
        nToggleOverlay = 1;
    }  
    if( arg.key == OIS::KC_F2)
    {
        nToggleOverlay = 2;
    }  
    if( arg.key == OIS::KC_F3)
    {
        nToggleOverlay = 3;
    }  
    if( arg.key == OIS::KC_F4)
    {
        nToggleOverlay = 4;
    }  
    if( arg.key == OIS::KC_F5)
    {
        nToggleOverlay = 5;
    }  

    std::cout << " KeyReleased {" << arg.key
        << ", " << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key)
        << "} || Character (" << (char)arg.text << ")" << std::endl;
    return true;
}
bool 
InputScene::mouseMoved( const OIS::MouseEvent &arg ) 
{
    const OIS::MouseState& s = arg.state;    
    std::cout << "\nMouseMoved: Abs("
              << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
              << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
    return true;
}
bool 
InputScene::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) 
{
    const OIS::MouseState& s = arg.state;
    
    if ( id == OIS::MB_Right)
    {
        m_MouseMoveEnabled = True;
        m_nRotateLeftRight = s.Y.rel;
        m_nRotateUpDown = s.X.rel;
    }
    
    std::cout << "\nMouse button #" << id << " pressed. Abs("
              << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
              << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
    return true;
}
bool 
InputScene::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) 
{
    const OIS::MouseState& s = arg.state;
    if ( id == OIS::MB_Right)
    {
        m_MouseMoveEnabled = False;
    }
    
    std::cout << "\nMouse button #" << id << " released. Abs("
              << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
              << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
    return true;
}
