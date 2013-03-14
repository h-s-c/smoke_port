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

class FMODSystem;
class FMODTask;
class FMODObject;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>FMODScene</c> Implementation of the ISystemScene interface.
///   The sound scene contains all the sound objects and info relevent to all 
///    sounds.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class FMODScene : public ISystemScene
{
    friend FMODSystem;
    friend FMODTask;

protected:

    FMODScene( ISystem* pSystem );
    ~FMODScene( void );

    /// <summary cref="FMODScene::GetSystemType">
    ///   Implementation of the <c>ISystemScene::GetSystemType</c> function.
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemScene::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="FMODScene::Update">
    /// This function must be called every frame.  It updates the sound scene.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="FMODScene::Initialize">
    ///   Implementation of the <c>ISystemScene::Initialize</c> function.
    ///   One time initialization function for the scene.
    /// </summary>
    /// <param name="Properties">Initializes the sound scene with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystemScene::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="FMODScene::GetProperties">
    ///   Implementation of the <c>ISystemScene::GetProperties</c> function.
    ///   Gets the properties of this scene.
    /// </summary>
    /// <param name="Properties">Gets the properties of the sound scene</param>
    /// <seealso cref="ISystemScene::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="FMODScene::SetProperties">
    ///   Implementation of the <c>ISystemScene::SetProperties</c> function.
    ///   Sets the properties for this scene.
    /// </summary>
    /// <param name="Properties">Sets the properties of the sound scene</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="FMODScene::GetObjectTypes">
    ///   Implementation of the <c>ISystemScene::GetObjectTypes</c> function.
    ///   Get all the available object types as names.
    /// </summary>
    /// <returns>pcstr* - A NULL terminated array of object type names.</returns>
    /// <seealso cref="ISystemScene::GetObjectTypes"/>
    virtual pcstr* GetObjectTypes( void );

    /// <summary cref="FMODScene::CreateObject">
    ///   Implementation of the <c>ISystemScene::CreateObject</c> function.
    ///   Creates a system object used to extend a UObject.
    /// </summary>
    /// <param name="pszName">The unique name for this object.</param>
    /// <param name="pszType">The object type to create.</param>
    /// <returns>ISystemObject* - The newly created system object.</returns>
    /// <seealso cref="ISystemScene::CreateObject"/>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );

    /// <summary cref="FMODScene::DestroyObject">
    ///   Implementation of the <c>ISystemScene::DestroyObject</c> function.
    ///   Destroys a system object.
    /// </summary>
    /// <param name="pSystemObject">The system object to destroy.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystemScene::DestroyObject"/>
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    /// <summary cref="FMODScene::GetSystemTask">
    ///   Implementation of the <c>ISystemScene::GetSystemTask</c> function.
    ///   Returns a pointer to the task that this scene needs to perform on its objects.
    /// </summary>
    /// <returns>ISystemTask* - The task for this scene.</returns>
    /// <seealso cref="ISystemScene::GetSystemTask"/>
    virtual ISystemTask* GetSystemTask( void );

    /// <summary cref="FMODScene::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identies the system changes that this subject could possibly make.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the possible system changes.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="FMODScene::GetDesiredSystemChanges">
    ///   Implementation of the <c>ISystemScene::GetDesiredSystemChanges</c> function.
    ///   Returns a bit mask of System Changes that this scene wants to receive changes for.  
    ///   Used to inform the change control manager if this scene should be informed of the
    ///   change.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the desired system changes.</returns>
    /// <seealso cref="ISystemScene::GetDesiredSystemChanges"/>
    virtual System::Changes::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="FMODScene::ChangeOccurred">
    ///   Implementation of the <c>ISystemScene::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject"> A pointer to the ISubject interface of the component that changed.</param>
    /// <param name="ChangeType"> The aspects of interest that changed as defined by the supplied 
    ///        ISubject's published interest bits.  If uInChangeBits are 0, then the subject is 
    //         shutting down, and should be released. 
    /// </param>
    /// <seealso cref="ISystemScene::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

protected:

    FMODTask*              m_pFMODTask; // Main task for this scen
    std::list<FMODObject*> m_Objects;   // Scene objects
};

