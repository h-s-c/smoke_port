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


#include "Physics\Collide\Shape\Compound\Collection\SimpleMesh\hkpSimpleMeshShape.h"


class HavokPhysicsSystem;
class HavokPhysicsTask;
class HavokPhysicsObject;
class HavokCharacterObject;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>HavokPhysicsScene</c> Implementation of the ISystemScene interface.
///   The HavokPhysics scene contains all objects and info relevent to all objects.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class HavokPhysicsScene : public ISystemScene, public IGenericScene, public hkpBreakOffPartsListener
{
    friend HavokPhysicsSystem;
    friend HavokPhysicsTask;


protected:

    HavokPhysicsScene( ISystem* pSystem );
    ~HavokPhysicsScene( void );

    /// <summary cref="HavokPhysicsScene::GetSystemType">
    ///   Implementation of the <c>ISystemScene::GetSystemType</c> function.
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemScene::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="HavokPhysicsScene::GlobalSceneStatusChanged">
    ///   Implementation of the <c>ISystemScene::GlobalSceneStatusChanged</c> function.
    ///   Called from the framework to inform the scene extension of the overall scene status.
    /// </summary>
    /// <param name="Status">New global scene status</param>
    /// <seealso cref="ISystemScene::GlobalSceneStatusChanged"/>
    virtual void GlobalSceneStatusChanged( GlobalSceneStatus Status );

    /// <summary cref="HavokPhysicsScene::Initialize">
    ///   Implementation of the <c>ISystemScene::Initialize</c> function.
    ///   One time initialization function for the scene.
    /// </summary>
    /// <param name="Properties">Initializes the scene with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="ISystemScene::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="HavokPhysicsScene::GetProperties">
    ///   Implementation of the <c>ISystemScene::GetProperties</c> function.
    ///   Gets the properties of this scene.
    /// </summary>
    /// <param name="Properties">Gets the properties of the scene</param>
    /// <seealso cref="ISystemScene::GetProperties"/>
    virtual void GetProperties( std::vector<Properties::Property>& Properties );

    /// <summary cref="HavokPhysicsScene::SetProperties">
    ///   Implementation of the <c>ISystemScene::SetProperties</c> function.
    ///   Sets the properties for this scene.
    /// </summary>
    /// <param name="Properties">Sets the properties of the scene</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( std::vector<Properties::Property> Properties );

    /// <summary cref="HavokPhysicsScene::GetObjectTypes">
    ///   Implementation of the <c>ISystemScene::GetObjectTypes</c> function.
    ///   Get all the available object types as names.
    /// </summary>
    /// <returns>pcstr* - A NULL terminated array of object type names.</returns>
    /// <seealso cref="ISystemScene::GetObjectTypes"/>
    virtual pcstr* GetObjectTypes( void );

    /// <summary cref="HavokPhysicsScene::CreateObject">
    ///   Implementation of the <c>ISystemScene::CreateObject</c> function.
    ///   Creates a system object used to extend a UObject.
    /// </summary>
    /// <param name="pszName">The unique name for this object.</param>
    /// <param name="pszType">The object type to create.</param>
    /// <returns>ISystemObject* - The newly created system object.</returns>
    /// <seealso cref="ISystemScene::CreateObject"/>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );

    /// <summary cref="HavokPhysicsScene::DestroyObject">
    ///   Implementation of the <c>ISystemScene::DestroyObject</c> function.
    ///   Destroys a system object.
    /// </summary>
    /// <param name="pSystemObject">The system object to destroy.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystemScene::DestroyObject"/>
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    /// <summary cref="HavokPhysicsScene::GetSystemTask">
    ///   Implementation of the <c>ISystemScene::GetSystemTask</c> function.
    ///   Returns a pointer to the task that this scene needs to perform on its objects.
    /// </summary>
    /// <returns>ISystemTask* - The task for this scene.</returns>
    /// <seealso cref="ISystemScene::GetSystemTask"/>
    virtual ISystemTask* GetSystemTask( void );

    /// <summary cref="HavokPhysicsScene::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identies the system changes that this subject could possibly make.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the possible system changes.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="HavokPhysicsScene::GetCreateObjects">
    ///   Implementation of the <c>ISystemScene::GetCreateObjects</c> function.
    ///   Returns the creation data of all the universal objects to create.
    ///   (Note: Not implemented)
    /// </summary>
    /// <param name="apszNames">Array of names to fill in.</param>
    /// <seealso cref="ISystemScene::GetCreateObjects"/>
    virtual void GetCreateObjects( CreateObjectDataArray& apszNames );

    /// <summary cref="HavokPhysicsScene::GetDestroyObjects">
    ///   Implementation of the <c>ISystemScene::GetDestroyObjects</c> function.
    ///   Returns the names of all the universal objects to destroy.
    ///   (Note: Not implemented)
    /// </summary>
    /// <param name="apszNames">Array of names to fill in.</param>
    /// <seealso cref="ISystemScene::GetDestroyObjects"/>
    virtual void GetDestroyObjects( std::vector<pcstr>& apszNames );

    /// <summary cref="HavokPhysicsScene::GetExtendObjects">
    ///   Implementation of the <c>ISystemScene::GetExtendObjects</c> function.
    ///   Returns the names of all the universal objects to extend.
    /// </summary>
    /// <param name="apszNames">Array of names to fill in.</param>
    /// <seealso cref="ISystemScene::GetExtendObjects"/>
    virtual void GetExtendObjects( ExtendObjectDataArray& apszNames );

    /// <summary cref="HavokPhysicsScene::GetUnextendObjects">
    ///   Implementation of the <c>ISystemScene::GetUnextendObjects</c> function.
    ///   Returns the names of all the universal objects to unextend.
    ///   (Note: Not implemented)
    /// </summary>
    /// <param name="apszNames">Array of names to fill in.</param>
    /// <seealso cref="ISystemScene::GetUnextendObjects"/>
    virtual void GetUnextendObjects( std::vector<pcstr>& apszNames );

    /// <summary cref="HavokPhysicsScene::ExtendObject">
    ///   Implementation of the <c>ISystemScene::ExtendObject</c> function.
    ///   Returns the universal object extension.
    /// </summary>
    /// <param name="pszName">Name of new object.</param>
    /// <param name="pUserData">Pointer to a hkpRigidBody used to create the object.</param>
    /// <returns>ISystemObject* - A pointer to the newly created (extened) object.</returns>
    /// <seealso cref="ISystemScene::ExtendObject"/>
    virtual ISystemObject* ExtendObject( pcstr pszName, void* pUserData );

    /// <summary cref="HavokPhysicsScene::UnextendObject">
    ///   Implementation of the <c>ISystemScene::UnextendObject</c> function.
    ///   Informs the ISystemScene of the object extension being removed.
    ///   (Note: Not implemented)
    /// </summary>
    /// <param name="pszName">Name of object being unextended.</param>
    /// <seealso cref="ISystemScene::UnextendObject"/>
    virtual ISystemObject* UnextendObject( pcstr pszName );

    /// <summary cref="HavokPhysicsScene::breakOffSubPart">
    ///   Callback when a piece needs to be broken off
    /// </summary>
    virtual hkResult breakOffSubPart( const ContactImpulseLimitBreachedEvent& Event,
                                      hkArray<hkpShapeKey>& KeysBrokenOff,
                                      hkpPhysicsSystem &PhysicsSystem ); 


public:

    /// <summary cref="HavokPhysicsScene::GetWorld">
    ///   Gets the Havok World used by this scene.
    /// </summary>
    /// <returns>hkpWorld* - A pointer this scenes Havok World.</returns>
    hkpWorld* GetWorld( void )
    {
        ASSERT( m_pWorld != NULL );
        return m_pWorld;
    }

    /// <summary cref="HavokPhysicsScene::GetTask">
    ///   Gets the Task associated with this scene.
    /// </summary>
    /// <returns>HavokPhysicsTask* - A pointer this scenes Task.</returns>
    HavokPhysicsTask* GetTask( void )
    {
        return m_pTask;
    }


    //const Base::Vector3& GetGravity( void )
    //{
    //    return m_Gravity;
    //}

    /// <summary cref="HavokPhysicsScene::GetCharacters">
    ///   Gets the all Character objects in this scene.
    /// </summary>
    /// <returns>std::list - A list of Character objects in the scene.</returns>
    inline std::list<HavokCharacterObject*> GetCharacters( void ) { return m_Characters; }

    /// <summary cref="HavokPhysicsScene::GetMaterialId">
    ///   Returns the ID for the given material name.
    /// </summary>
    /// <param name="sName">Material name.</param>
    /// <returns>i32 - Unique ID associated with the given name.</returns>
    i32 GetMaterialId( const std::string& sName );

    /// <summary cref="HavokPhysicsScene::AddTrackCollision">
    ///   Adds a collision relationship to track and report.
    /// </summary>
    /// <param name="pSubject">Subject that can cause collisions.</param>
    /// <param name="pObserver">Observer that wants to hear about collisions.</param>
    void AddTrackCollision( HavokPhysicsObject* pSubject, HavokPhysicsObject* pObserver );

    /// <summary cref="HavokPhysicsScene::RemoveTrackCollision">
    ///   Removes a collision relationship to track and report
    /// </summary>
    /// <param name="pSubject">Subject that could have cause collisions.</param>
    /// <param name="pObserver">Observer that wanted to hear about collisions.</param>
    void RemoveTrackCollision( HavokPhysicsObject* pSubject, HavokPhysicsObject* pObserver );

protected:

    static const Base::Vector3          sm_kDefaultGravity;

    const hkVector4                     m_kVelocityModifier;
    const f32                           m_kfMass;

    enum PropertyTypes
    {
        Property_SceneFile,
        Property_Gravity,
        Property_Material, Property_Elasticity, Property_Friction, Property_Softness,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    std::list<HavokCharacterObject*>    m_Characters;

    HavokPhysicsTask*                   m_pTask;

    btDiscreteDynamicsWorld*            m_pWorld;
    //Base::Vector3                       m_Gravity;

    struct Material
    {
        std::string                     Name;
        i32                             Id;
    };
    typedef std::vector<Material>       Materials;
    Materials                           m_Materials;

    void*                               m_pLoader;

    enum SceneFileTypes
    {
        Extend, Loose, BreakOffParts,
    };

    struct SceneFileData
    {
        std::string                     sPathName;
        SceneFileTypes                  Type;
        f32                             Value1;
        f32                             Value2;
    };
    typedef std::vector<SceneFileData>  SceneFiles;
    SceneFiles                          m_SceneFiles;

    struct ExtensionData
    {
        pcstr                           pszName;
        hkpRigidBody*                   pBody;
    };
    std::map<void*, ExtensionData>      m_aExtensions;

    hkArray<hkpRigidBody*>              m_aUnattachedBodies;

    hkArray<hkpBreakOffPartsUtil*>      m_apBreakOffPartsUtils;

    SpinWait                            m_BrokenOffPartsSpinWait;

    typedef std::map<HavokPhysicsObject*, std::vector<HavokPhysicsObject*> > CollisionTracker;
    typedef CollisionTracker::iterator  CollisionTrackerIt;
    CollisionTracker                    m_CollisionTracker;
};

