#pragma once

#include <mutex>

#include "Systems/GraphicsOGRE/Extras/PagedGeometry.hpp"
#include "Systems/GraphicsOGRE/Extras/GrassLoader.hpp"


class OGREGraphicsSystem;
class OGREGraphicsTask;
class OGREGraphicsObject;

// Macro to convert from Base::Vector3 to Ogre::Vector3
#define TOOGREVEC(Vec)  (Ogre::Vector3((Vec).x, (Vec).y, (Vec).z) )
// Macro to convert from Base::Quaternion to Ogre::Quaternion
#define TOOGREQUAT(Vec) (Ogre::Quaternion((Vec).w, (Vec).x, (Vec).y, (Vec).z) )

#define MAX_NUM_JOBS 32

struct ProcessData
{
    std::list<OGREGraphicsObject*> m_Objects;    // Objects to process
    f32                            m_DeltaTime;  // Delta time (needed for Update calls)
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implementation of the ISystemScene interface.
///   See Interfaces\System.h for a definition of the class and its functions.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class OGREGraphicsScene : public ISystemScene
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsTask;

public:
    std::mutex m_mutex;

public:

    /// <summary cref="OGREGraphicsScene::Update">
    ///   This function must be called every frame.  It updates the graphics scene.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="OGREGraphicsScene::Update">
    ///   This function returns a pointer to the interanl Ogre scene manager.
    /// </summary>
    /// <returns>Ogre::SceneManager* - A pointer to the Ogre scene manager.</returns>
    Ogre::SceneManager* GetOGRESceneManager( void )
    {
        return m_pSceneManager;
    }

    /// <summary cref="OGREGraphicsScene::GetOGRERootSceneNode">
    ///   This function returns a pointer to the interanl Ogre scene root node.
    /// </summary>
    /// <returns>Ogre::SceneNode* - A pointer to the Ogre scene root node.</returns>
    Ogre::SceneNode* GetOGRERootSceneNode( void )
    {
        return m_pRootNode;
    }

    /// <summary cref="OGREGraphicsScene::GetOGREOverlayManager">
    ///   This function returns a pointer to the interanl Ogre overlay manager.
    /// </summary>
    /// <returns>Ogre::OverlayManager* - A pointer to the Ogre overlay manager.</returns
    Ogre::OverlayManager* GetOGREOverlayManager( void )
    {
        return m_pOverlayManager;
    }

    /// <summary cref="OGREGraphicsScene::GetDefaultOverlay">
    ///   This function returns a pointer to the default overlay.
    /// </summary>
    /// <returns>Ogre::Overlay* - A pointer to the default overlay.</returns
    Ogre::Overlay* GetDefaultOverlay( void )
    {
        return m_pOverlay;
    }

    /// <summary cref="OGREGraphicsScene::GetGrassLoader">
    ///   This function returns a pointer to the interanl paged geometry grass loader.
    /// </summary>
    /// <returns>Forests::GrassLoader* - A pointer to the interal grass loader.</returns
    Forests::GrassLoader* GetGrassLoader( void )
    {
        return m_pGrassLoader;
    }

    /// <summary cref="OGREGraphicsScene::SetCamera">
    ///   This function sets all the internal data for the given camera.
    /// </summary>
    /// <param name="pCamera">Ogre::Camera* - A pointer to the new camera.</param>
    void SetCamera( Ogre::Camera *pCamera )
    {
        m_pPagedGeometry->setCamera( pCamera );
    }

    /// <summary cref="OGREGraphicsScene::SetDetailLevel">
    ///   Updates the internal setting for detail level.
    ///   Note: The detail level must be set after the camera and other settings
    /// </summary>
    void SetDetailLevel( void )
    {
        m_pPagedGeometry->addDetailLevel<Forests::GrassPage>(m_fDrawDistance);
    }

    /// <summary cref="OGREGraphicsScene::getTerrainHeightScene">
    ///   Gets the height from the height map at the given a,b offset.
    /// </summary>
    /// <param name="a">const float - Horizontal offset into height map.</param>
    /// <param name="b">const float - Vertical offset into height map.</param>
    /// <param name="userData">void* - Pointer to user data used for the height lookup.</param>
    /// <returns>float - Height for the give params in scene units.</returns
    float getTerrainHeightScene(const float a, const float b, void *userData);

protected:

    OGREGraphicsScene( ISystem* pSystem );
    ~OGREGraphicsScene( void );

    /// <summary cref="OGREGraphicsScene::GetSystemType">
    ///   Implementation of the <c>ISystemScene::GetSystemType</c> function.
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemScene::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="OGREGraphicsScene::GlobalSceneStatusChanged">
    ///   Called from the framework to inform the scene extension of the overall scene status.
    /// </summary>
    /// <param name="Status">GlobalSceneStatus - The overall scene status.</param>
    virtual void GlobalSceneStatusChanged( GlobalSceneStatus Status );

    /// <summary cref="OGREGraphicsScene::Initialize">
    ///   Implementation of the <c>ISystemScene::Initialize</c> function.
    ///   One time initialization function for the scene.
    /// </summary>
    /// <param name="Properties">Initializes the scene with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystemScene::Initialize"/>
    virtual Error Initialize( Properties::Array Properties );

    /// <summary cref="OGREGraphicsScene::GetProperties">
    ///   Implementation of the <c>ISystemScene::GetProperties</c> function.
    ///   Gets the properties of this scene.
    /// </summary>
    /// <param name="Properties">Gets the properties of the scene</param>
    /// <seealso cref="ISystemScene::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsScene::SetProperties">
    ///   Implementation of the <c>ISystemScene::SetProperties</c> function.
    ///   Sets the properties for this scene.
    /// </summary>
    /// <param name="Properties">Sets the properties of the scene</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsScene::GetObjectTypes">
    ///   Implementation of the <c>ISystemScene::GetObjectTypes</c> function.
    ///   Get all the available object types as names.
    /// </summary>
    /// <returns>pcstr* - A NULL terminated array of object type names.</returns>
    /// <seealso cref="ISystemScene::GetObjectTypes"/>
    virtual pcstr* GetObjectTypes( void );

    /// <summary cref="OGREGraphicsScene::CreateObject">
    ///   Implementation of the <c>ISystemScene::CreateObject</c> function.
    ///   Creates a system object used to extend a UObject.
    /// </summary>
    /// <param name="pszName">The unique name for this object.</param>
    /// <param name="pszType">The object type to create.</param>
    /// <returns>ISystemObject* - The newly created system object.</returns>
    /// <seealso cref="ISystemScene::CreateObject"/>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );

    /// <summary cref="OGREGraphicsScene::DestroyObject">
    ///   Implementation of the <c>ISystemScene::DestroyObject</c> function.
    ///   Destroys a system object.
    /// </summary>
    /// <param name="pSystemObject">The system object to destroy.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystemScene::DestroyObject"/>
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    /// <summary cref="OGREGraphicsScene::GetSystemTask">
    ///   Implementation of the <c>ISystemScene::GetSystemTask</c> function.
    ///   Returns a pointer to the task that this scene needs to perform on its objects.
    /// </summary>
    /// <returns>ISystemTask* - The task for this scene.</returns>
    /// <seealso cref="ISystemScene::GetSystemTask"/>
    virtual ISystemTask* GetSystemTask( void );

    /// <summary cref="OGREGraphicsScene::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identies the system changes that this subject could possibly make.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the possible system changes.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="OGREGraphicsScene::GetDesiredSystemChanges">
    ///   Implementation of the <c>ISystemScene::GetDesiredSystemChanges</c> function.
    ///   Returns a bit mask of System Changes that this scene wants to receive changes for.  
    ///   Used to inform the change control manager if this scene should be informed of the
    ///   change.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the desired system changes.</returns>
    /// <seealso cref="ISystemScene::GetDesiredSystemChanges"/>
    virtual const void* GetSystemChangeData( System::Change SystemChange );

public:
    typedef std::vector<OGREGraphicsObject*>    ObjectsList;

protected:

    enum PropertyTypes
    {
        Property_ResourceLocation, Property_DelResourceLocation,
        Property_AmbientLight, Property_Shadows, 
        Property_ShadowColor, Property_DrawBoundingBox, 
        Property_ShowNormals, Property_ShowTangents,
        Property_UseStaticGeom, Property_UseInstancedGeom,
        Property_FogColor, Property_Fog, Property_Font,
        Property_PagedGeometry, Property_PagedGeometryTerrain,
        Property_PagedGeometryTerrainOffset,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    ObjectsList                         m_Objects;
    std::map<std::string, Ogre::StaticGeometry*>    m_StaticGeoms;
    std::map<std::string, std::vector<Ogre::InstancedGeometry*> > m_InstancedGeoms;

    OGREGraphicsTask*                   m_pTask;

    Ogre::SceneManager*                 m_pSceneManager;
    Ogre::SceneNode*                    m_pRootNode;

    Ogre::OverlayManager*               m_pOverlayManager;
    Ogre::Overlay*                      m_pOverlay;

    Bool                                m_bUseStaticGeom;
    Bool                                m_bUseInstancedGeom;

    ProcessData                         m_ProcessData[ MAX_NUM_JOBS ];

    Ogre::ColourValue                   m_FogColor;
    Ogre::FogMode                       m_FogMode;
    float                               m_ExpDensity;
    float                               m_LinearStart;
    float                               m_LinearEnd;

    //
    // PagedGeometry
    //
    Forests::PagedGeometry             *m_pPagedGeometry;
    Forests::GrassLoader               *m_pGrassLoader;
    Ogre::Image                         m_HeightMapImage;
    std::string                         m_sHeightmap;
    std::string                         m_sResourceGroup;
    f32                                 m_fPageSize;
    f32                                 m_fDrawDistance;
    f32                                 m_fTerrainWidth;
    f32                                 m_fTerrainLength;
    f32                                 m_fTerrainHeight;
    f32                                 m_fTerrainWidthOffset;
    f32                                 m_fTerrainLengthOffset;
    f32                                 m_fTerrainHeightOffset;

    Bool    m_bParallelize;
    Bool    m_bPause;
    f32     m_fDeltaTime;

    /// <summary cref="OGREGraphicsScene::UpdateCallback">
    ///   Invoked by ParalellFor algorithm to update a range of objects.
    /// </summary>
    static void UpdateCallback( void *param, u32 begin, u32 end );

    /// <summary cref="OGREGraphicsScene::UpdateCallback">
    ///   Updates the given range of fire objects.
    /// </summary>
    void ProcessRange( u32 begin, u32 end );
};

