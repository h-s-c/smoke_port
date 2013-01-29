#pragma once

class OGREGraphicsSystem;
class OGREGraphicsScene;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implementation of the IGraphicsObject interface.
///   See Interfaces\Graphics.h and Interfaces\System.h for a definition of the
///   class and its functions.
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class OGREGraphicsObjectCamera : public OGREGraphicsObject, public IGeometryObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;
    friend OGREGraphicsTask;

protected:

    OGREGraphicsObjectCamera( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectCamera( void );

    /// <summary cref="OGREGraphicsObjectCamera::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectCamera::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectCamera::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectCamera::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectCamera::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="OGREGraphicsObjectCamera::UpdatePolygonMode">
    ///   Updates the polygon mode based on m_PolygonMode.
    /// </summary>
    void UpdatePolygonMode( void );

    /// <summary cref="OGREGraphicsObjectCamera::Update">
    ///   Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="OGREGraphicsObject::Update"/>
    void Update( f32 DeltaTime );

public:
    /// <summary cref="OGREGraphicsObjectCamera::GetPosition">
    ///   Implementation of the IGeometryObject GetPosition function.
    /// </summary>
    /// <seealso cref="IGeometryObject::GetPosition"/>
    virtual const Math::Vector3* GetPosition( void );

    /// <summary cref="OGREGraphicsObjectCamera::GetOrientation">
    ///   Implementation of the IGeometryObject GetOrientation function.
    /// </summary>
    /// <seealso cref="IGeometryObject::GetOrientation"/>
    virtual const Math::Quaternion* GetOrientation( void );

    /// <summary cref="OGREGraphicsObjectCamera::GetScale">
    ///   Implementation of the IGeometryObject GetScale function.
    /// </summary>
    /// <seealso cref="IGeometryObject::GetScale"/>
    virtual const Math::Vector3* GetScale( void );

    /// <summary cref="OGREGraphicsObjectCamera::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );


protected:

    //
    // Camera Graphics Object
    //
    enum PropertyTypes
    {
        Property_FOVy, Property_ClipDistances,
        Property_Position, Property_LookAt,
        Property_PolygonMode, Property_LockCamera,
        Property_PagedGeometry,
        Property_Count
    };

    static const pcstr                  sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Ogre::Camera*                       m_pCamera;
    Ogre::Viewport*                     m_pViewport;

    bool                                m_bLocked;
public:
    Math::Vector3                       m_vLookAt;
protected:
    System::Types::BitMask              m_Modified;

    enum PolygonModes
    {
        PolygonMode_Invalid=-1,

        PolygonMode_Points, PolygonMode_WireFrame, PolygonMode_Solid,
        PolygonMode_Count
    };

    static const pcstr                  sm_kapszPolygonModeEnumOptions[];
    PolygonModes                        m_PolygonMode;
public:
    Math::Vector3                       m_Position;     // Position of AI object
protected:
    Math::Quaternion                    m_Orientation;  // Orientation of AI object
    Math::Vector3                       m_Scale;        // Scale of AI object
};

