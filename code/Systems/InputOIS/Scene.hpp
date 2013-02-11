#pragma once

//extern
#include <OISInputManager.h>
#include <OISException.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OISEvents.h>
//system
#include "Systems/InputOIS/System.hpp"

class InputSystem;
class InputTask;
class InputObject;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>InputScene</c> Implementation of the ISystemScene interface.
///   The input scene contains all the input objects
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class InputScene : public ISystemScene, public OIS::KeyListener, public OIS::MouseListener
{
    friend class InputSystem;
    friend class InputTask;

protected:

    InputScene( ISystem* pSystem );
    ~InputScene( void );

    /// <summary cref="InputScene::GetSystemType">
    ///   Implementation of the <c>ISystemScene::GetSystemType</c> function.
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemScene::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="InputScene::InputUpdate">
    /// This function must be called every frame.  It updates the input scene.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    void InputUpdate(f32 DeltaTime);
    
    /// <summary cref="InputScene::Initialize">
    ///   Implementation of the <c>ISystemScene::Initialize</c> function.
    ///   One time initialization function for the scene.
    /// </summary>
    /// <param name="Properties">Initializes the input scene with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystemScene::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );
    
    /// <summary cref="InputScene::GetProperties">
    ///   Implementation of the <c>ISystemScene::GetProperties</c> function.
    ///   Gets the properties of this scene.
    /// </summary>
    /// <param name="Properties">Gets the properties of the sound scene</param>
    /// <seealso cref="ISystemScene::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );
    
    /// <summary cref="InputScene::SetProperties">
    ///   Implementation of the <c>ISystemScene::SetProperties</c> function.
    ///   Sets the properties for this scene.
    /// </summary>
    /// <param name="Properties">Sets the properties of the input scene</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );
    
    /// <summary cref="InputScene::GetObjectTypes">
    ///   Implementation of the <c>ISystemScene::GetObjectTypes</c> function.
    ///   Get all the available object types as names.
    /// </summary>
    /// <returns>pcstr* - A NULL terminated array of object type names.</returns>
    /// <seealso cref="ISystemScene::GetObjectTypes"/>
    virtual pcstr* GetObjectTypes( void );
    
    /// <summary cref="InputScene::CreateObject">
    ///   Implementation of the <c>ISystemScene::CreateObject</c> function.
    ///   Creates a system object used to extend a UObject.
    /// </summary>
    /// <param name="pszName">The unique name for this object.</param>
    /// <param name="pszType">The object type to create.</param>
    /// <returns>ISystemObject* - The newly created system object.</returns>
    /// <seealso cref="ISystemScene::CreateObject"/>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );
    
    /// <summary cref="InputScene::DestroyObject">
    ///   Implementation of the <c>ISystemScene::DestroyObject</c> function.
    ///   Destroys a system object.
    /// </summary>
    /// <param name="pSystemObject">The system object to destroy.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystemScene::DestroyObject"/>
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    /// <summary cref="InputScene::GetSystemTask">
    ///   Implementation of the <c>ISystemScene::GetSystemTask</c> function.
    ///   Returns a pointer to the task that this scene needs to perform on its objects.
    /// </summary>
    /// <returns>ISystemTask* - The task for this scene.</returns>
    /// <seealso cref="ISystemScene::GetSystemTask"/>
    virtual ISystemTask* GetSystemTask( void );

    /// <summary cref="InputScene::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identifies the system changes that this subject could possibly make.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the possible system changes.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="InputScene::GetSystemChangeData">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identifies the system changes that this subject could possibly make.
    /// </summary>
    /// <param name="SystemChange">A bitmask of the changes in the input scene.</param>
    virtual const void* GetSystemChangeData( System::Change SystemChange );

protected:

    // These three variables are used for determining what hotkey combination was used
    InputSystem*                    m_pInputSystem;

    // Main task for the input scene
    InputTask*                      m_pInputTask;

    // Input scene objects
    std::list<InputObject*>         m_Objects;

    // Enable catching mouse movements
    Bool                            m_MouseMoveEnabled;

    f32                             m_nRotateLeftRight;
    f32                             m_nRotateUpDown;
    f32                             m_nMoveLeftRight;
    f32                             m_nMoveForwardBack;
    f32                             m_nMoveUpDown;

    //
    // For the keyboard, we need to save the amount moved on a keydown so that it can be reversed on a keyup.  We
    // cannot recalculate because DeltaTime is slightly different every update so you will end up with drifting.
    //
    f32                             m_nOldForward;
    f32                             m_nOldBack;
    f32                             m_nOldLeft;
    f32                             m_nOldRight;
    f32                             m_nOldUp;
    f32                             m_nOldDown;

private:    
    OIS::InputManager*              m_InputManager;
    OIS::Mouse*                     m_Mouse;
    OIS::Keyboard*                  m_Keyboard;
    
    void                            handleNonBufferedKeys();
    void                            handleNonBufferedMouse();
    bool                            keyPressed( const OIS::KeyEvent &arg );
    bool                            keyReleased( const OIS::KeyEvent &arg );
    bool                            mouseMoved( const OIS::MouseEvent &arg );
    bool                            mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool                            mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
       
    
    bool bUseFireHose = false;
    bool bScenePaused = false;
    
    int nToggleOverlay = 0;

};
