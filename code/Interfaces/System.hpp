// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#pragma once

// VS2010 support
#if !(_MSC_VER < 1700)
#include <functional>
#endif

// Forward declarations
class ISystem;
class ISystemScene;
class ISystemObject;
class ISystemTask;
class ITaskManager;

////////////////////////////////////////////////////////////////////////
///   ISystem is an interface class designed to be a method for adding 
///   functionality to the framework.  By default the framework does not 
///   have functionality for things like graphics, physics, etc.
////////////////////////////////////////////////////////////////////////

class ISystem
{
public:
    // Constructor.
    ISystem( void ) : m_bInitialized( False){}

    // Gets the name of the system.  
    // Only custom systems can return a custom name.
    virtual pcstr GetName( void ) = 0;

    // Gets the system type for this system.
    virtual System::Type GetSystemType( void ) = 0;

    // One time initialization function for the system.
    virtual Error Initialize( 
                    std::vector<Properties::Property> Properties ) = 0;

    //   Gets the properties of this system.
    virtual void GetProperties( 
                    std::vector<Properties::Property>& Properties ) = 0;

    //   Sets the properties for this system.
    virtual void SetProperties( 
                    std::vector<Properties::Property> Properties ) = 0;

    // Creates a system scene for containing system objects.
    virtual ISystemScene* CreateScene( void ) = 0;

    // Destroys a system scene.
    virtual Error DestroyScene( ISystemScene* pSystemScene ) = 0;
    
    // Returns the CPU Utilization of FMOD. Default value is 0.
    virtual f32 GetCPUUsage( void ) { return 0; }


protected:

    Bool                        m_bInitialized;
};


////////////////////////////////////////////////////////////////////////
///   ISystemScene is an interface class for managing a scene or scenes 
///   in a system.
////////////////////////////////////////////////////////////////////////

class ISystemScene : public CSubject, public IObserver
{
    friend ISystem;


public:

    /// <summary>
    ///   Constructor.
    /// </summary>
    /// <param name="pSystem">The system this scene belongs to.</param>
    ISystemScene( ISystem* pSystem ): m_bInitialized( False )
                                      ,m_pSystem( pSystem )
    {
        ASSERT( m_pSystem != NULL );
    }


    /// <summary>
    ///   Gets the system this object belongs to.
    /// </summary>
    /// <returns>A pointer to the system.</returns>
    ISystem* GetSystem( void )
    {
        return m_pSystem;
    }

    /// <summary>
    ///   Gets the system type for this system scene.
    /// </summary>
    /// <remarks>
    ///   This is a shortcut to getting the system type w/o having to go the system first.
    /// </remarks>
    /// <returns>The type of the system.</returns>
    virtual System::Type GetSystemType( void ) = 0;

    /// <summary>
    ///   Enumeration for informing the ISystemScene on what is occuring.
    ///   <para>Pre and post loading are used to inform when the framework being and ends
    ///          completely loading a scene.</para>
    ///   <para>Pre and post destroying are used to inform when the framework being and ends
    ///          completely destroying a scene.</para>
    /// </summary>
    enum GlobalSceneStatus
    {
        PreLoadingObjects, PostLoadingObjects, PreDestroyingObjects, PostDestroyingObjects,
    };

    /// <summary>
    ///   Called from the framework to inform the scene extension of the overall scene status.
    /// </summary>
    /// <param name="Status">The overall scene status.</param>
    virtual void GlobalSceneStatusChanged( GlobalSceneStatus Status )
    {

    };

    /// <summary>
    ///   One time initialization function for the scene.
    /// </summary>
    /// <param name="Properties">Property structure array to fill in.</param>
    /// <returns>An error code.</returns>
    virtual Error Initialize( std::vector<Properties::Property> Properties ) = 0;

    /// <summary>
    ///   Gets the properties of this scene.
    /// </summary>
    /// <param name="Properties">Property structure array to fill.</param>
    virtual void GetProperties( std::vector<Properties::Property>& Properties ) = 0;

    /// <summary>
    ///   Sets the properties for this scene.
    /// </summary>
    /// <param name="Properties">Property structure array to get values from.</param>
    virtual void SetProperties( std::vector<Properties::Property> Properties ) = 0;

    /// <summary>
    ///   Get all the available object types as names.
    /// </summary>
    /// <returns>A NULL terminated array of object type names.</returns>
    virtual pcstr* GetObjectTypes( void ) = 0;

    /// <summary>
    ///   Creates a system object used to extend a UObject.
    /// </summary>
    /// <param name="pszName">The unique name for this object.</param>
    /// <param name="pszType">The object type to create.</param>
    /// <returns>The newly created system object.</returns>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType ) = 0;

    /// <summary>
    ///   Destroys a system object.
    /// </summary>
    /// <param name="pSystemObject">The system object to destroy.</param>
    /// <returns>An error code.</returns>
    virtual Error DestroyObject( ISystemObject* pSystemObject ) = 0;

    /// <summary>
    ///   Returns a pointer to the task that this scene needs to perform on its objects.
    /// </summary>
    /// <returns>The task for this scene.</returns>
    virtual ISystemTask* GetSystemTask( void ) = 0;

    /// <summary>
    ///   Returns a bit mask of System Changes that this scene wants to receive changes for.  Used
    ///    to inform the change control manager if this scene should be informed of the
    ///    change.
    /// </summary>
    /// <returns>A System::Changes::BitMask.</returns>
    virtual System::Changes::BitMask GetDesiredSystemChanges( void ) { return System::Changes::None; };

    /// <summary cref="IObserver::ChangeOccurred">
    ///   Implementation of the IObserver ChangeOccurred function.
    /// </summary>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
    {
        return Errors::Success;
    }

protected:

    Bool                        m_bInitialized;

    ISystem*                    m_pSystem;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>ISystemObject</c> is an interface class designed to be an extension of the UObject.
///   Systems can extend the UObject by implementing this interface to give it new  properties.
///   <para>
///     An example would be a physics system implementing this interface so that the UObject would
///     now be able to interact with the physics system.
///   </para>
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ISystemObject : public CSubject, public IObserver
{
    friend ISystem;
    friend ISystemScene;


public:

    /// <summary>
    ///   Constructor.
    /// </summary>
    /// <remarks>
    ///   Inlined for performance.
    /// </remarks>
    /// <param name="pSystemScene">The scene this object belongs to.</param>
    /// <param name="pszName">Name of this GUI object.</param>
    ISystemObject( ISystemScene* pSystemScene, pcstr pszName )
        : m_bInitialized( False )
        , m_pSystemScene( pSystemScene )
    {
        if( pszName )
        {
            SetName( pszName );
        }
    }

    /// <summary>
    ///   Gets the system scene this object belongs to.
    /// </summary>
    /// <returns>A pointer to the system.</returns>
    ISystemScene* GetSystemScene( void )
    {
        return m_pSystemScene;
    }

    /// <summary>
    ///   Returns a handle to the parent object that this system object belongs to.
    /// </summary>
    /// <returns>A handle to the parent object.</returns>
    Handle GetParentObject( void )
    {
        return m_hParentObject;
    }

    /// <summary>
    ///   Set a handle to the parent object that this system object belongs to.
    /// </summary>
    /// <remarks>This should only be called by the parent object.</remarks>
    /// <returns>A handle to the parent object.</returns>
    void SetParentObject( Handle hParentObject )
    {
        m_hParentObject = hParentObject;
    }

    /// <summary>
    ///   Gets the system type for this system object.
    /// </summary>
    /// <remarks>
    ///   This is a shortcut to getting the system type w/o having to go the system first.
    /// </remarks>
    /// <returns>The type of the system.</returns>
    virtual System::Type GetSystemType( void ) = 0;

    /// <summary>
    ///   Gets the name of the object.
    /// </summary>
    /// <returns>The name of the object.</returns>
    virtual inline pcstr GetName( void )
    {
        return m_sName.c_str();
    }

    /// <summary>
    ///   Sets the name of the object.
    /// </summary>
    /// <param name="pszName">The new name of the object.</param>
    virtual inline void SetName( pcstr pszName )
    {
        m_sName = pszName;
    }

    /// <summary>
    ///   One time initialization function for the object.
    /// </summary>
    /// <param name="Properties">Property structure array to fill in.</param>
    /// <returns>An error code.</returns>
    virtual Error Initialize( std::vector<Properties::Property> Properties ) = 0;

    /// <summary>
    ///   Gets the properties of this object.
    /// </summary>
    /// <param name="Properties">The Property structure array to fill.</param>
    virtual void GetProperties( std::vector<Properties::Property>& Properties ) = 0;

    /// <summary>
    ///   Sets the properties for this object.
    /// </summary>
    /// <param name="Properties">Property structure array to get values from.</param>
    virtual void SetProperties( std::vector<Properties::Property> Properties ) = 0;

    /// <summary>
    ///   Returns a bit mask of System Changes that this system wants to receive changes for.  Used
    ///    to inform the change control manager if this system's object should be informed of the
    ///    change.
    /// </summary>
    /// <returns>A System::Changes::BitMask.</returns>
    virtual System::Changes::BitMask GetDesiredSystemChanges( void ) = 0;


protected:

    Bool                        m_bInitialized;

    ISystemScene*               m_pSystemScene;

    Handle                      m_hParentObject;

    std::string                 m_sName;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>ISystemTask</c> is an interface class designed to work with a task manager for starting the
///    system's task and spawning off new tasks as need be.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ISystemTask
{
    friend ISystem;

public:

    /// <summary cref="ISystemTask::ISystemTask(ISystemScene*)">
    ///   Constructor.
    /// </summary>
    ISystemTask( ISystemScene* pSystemScene )
        : m_pSystemScene( pSystemScene )
    {
    }

    /// <summary cref="ISystemTask::GetSystemScene">
    ///   Gets the scene for this task.
    /// </summary>
    /// <returns>The scene for this task.</returns>
    ISystemScene* GetSystemScene( void )
    {
        return m_pSystemScene;
    }

    /// <summary cref="ISystemTask::GetSystemType">
    ///   Gets the system type for this system task.
    /// </summary>
    /// <remarks>
    ///   This is a shortcut to getting the system type without having to go the system first.
    /// </remarks>
    /// <returns>The type of the system.</returns>
    virtual System::Type GetSystemType( void ) = 0;

    /// <summary cref="ISystemTask::Update">
    ///   Function informing the task to perform its updates.
    /// </summary>
    /// <param name="DeltaTime">The time delta from the last call.</param>
    virtual void Update( f32 DeltaTime ) = 0;

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) = 0;

protected:

    ISystemScene*               m_pSystemScene;
};


//
// Function type definitions for communicating with the system DLL.
//
extern "C"
{
    struct ManagerInterfaces
    {
        IEnvironment*       pEnvironment;
        IService*           pService;
        ITaskManager*       pTask;
        IPlatform*          pPlatform;
    };
    
// VS2010 support
#if (_MSC_VER < 1700)
    typedef void (STDCALL* InitSystemFunction)(ManagerInterfaces* pManagers);
	typedef ISystem* (STDCALL* CreateSystemFunction)();
    typedef void (STDCALL* DestroySystemFunction)( ISystem* pSystem );
    struct SystemFuncs 
    {
        InitSystemFunction InitSystem;
        CreateSystemFunction CreateSystem;
        DestroySystemFunction DestroySystem;
    };   
#else 
    struct SystemFuncs 
    {
        std::function<void STDCALL (ManagerInterfaces* pManagers)> InitSystem;
        std::function<ISystem* STDCALL ()> CreateSystem;
        std::function<void STDCALL (ISystem* pSystem)> DestroySystem;
    };
#endif
}
