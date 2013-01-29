#pragma once


class OGREGraphicsSystem;
class OGREGraphicsScene;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>OGREGraphicsObjectParticleSystem</c>Graphics system object wrapping the Ogre ParticleFX particle system.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class OGREGraphicsObjectParticleSystem : public OGREGraphicsObject, public IGeometryObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;
    friend OGREGraphicsTask;

protected:

    /// <summary cref="OGREGraphicsObjectParticleSystem::OGREGraphicsObjectParticleSystem">
    /// ctor
    /// </summary>
    /// <param name="pSystemScene">Pointer to the <c>ISystemScene</c> object.</param>
    /// <param name="pszName">Name of the OGREGraphicsObjectParticleSystem object.</param>
    OGREGraphicsObjectParticleSystem( ISystemScene* pSystemScene, pcstr pszName );

    /// <summary cref="OGREGraphicsObjectParticleSystem::~OGREGraphicsObjectParticleSystem">
    /// dtor
    /// </summary>
    ~OGREGraphicsObjectParticleSystem( void );

    /// <summary cref="OGREGraphicsObjectParticleSystem::Initialize">
    /// Implementation of the <c>ISystem::Initialize</c> method.
    /// </summary>
    /// <param name="Properties">Property structure array to fill in</param>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectParticleSystem::GetProperties">
    /// Implementation of the <c>ISystem::GetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Get properties for this OGREGraphicsObjectParticleSystem object.</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectParticleSystem::SetProperties">
    /// Implementation of the <c>ISystem::SetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Set properties for this OGREGraphicsObjectParticleSystem object.</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectParticleSystem::GetDesiredSystemChanges">
    /// Implementation of the <c>ISubject::GetDesiredSystemChanges</c> method.
    /// </summary>
    /// <returns>Returns systems changes requested by this OGREGraphicsObjectParticleSystem object.
    /// </returns>
    /// <seealso cref="ISubject::GetDesiredSystemChanges"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectParticleSystem::GetPotentialSystemChanges">
    /// Implementation of the <c>ISubject::GetPotentialSystemChanges</c> method.
    /// </summary>
    /// <returns>Returns systems changes possible for this OGREGraphicsObjectParticleSystem object.
    /// </returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectParticleSystem::ChangeOccurred">
    /// Implementation of the <c>IObserver::ChangeOccurred</c> method.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error
    /// </returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="OGREGraphicsObjectParticleSystem::ChangeOccurred">
    /// Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    void Update( f32 DeltaTime );

public:
    /// <summary cref="OGREGraphicsObjectParticleSystem::GetPosition">
    /// Implementation of the <c>IGeometryObject::GetPosition</c> method.
    /// </summary>
    /// <returns>Position of the object.
    /// </returns>
    /// <seealso cref="IGeometryObject::GetPosition"/>
    virtual const Math::Vector3* GetPosition( void );
    /// <summary cref="OGREGraphicsObjectParticleSystem::GetOrientation">
    /// Implementation of the <c>IGeometryObject::GetOrientation</c> method.
    /// </summary>
    /// <returns>Orientation of the object.
    /// </returns>
    /// <seealso cref="IGeometryObject::GetOrientation"/>
    virtual const Math::Quaternion* GetOrientation( void );
    /// <summary cref="OGREGraphicsObjectParticleSystem::GetScale">
    /// Implementation of the <c>IGeometryObject::GetScale</c> method.
    /// </summary>
    /// <returns>Scale of the object.
    /// </returns>
    /// <seealso cref="IGeometryObject::GetScale"/>
    virtual const Math::Vector3* GetScale( void );

protected:

    static u32                          sm_EntityId;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object PropertyTypes type enum supported in this class.
    /// </summary >
    enum PropertyTypes
    {
        Property_PSystemType,
        Property_PSystemScripts, 
        Property_Count
    };

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object ParticleSystemType type enum denotes special handling for particle systems.
    /// </summary >
    enum ParticleSystemType
    {
        Type_FireHose,
        Type_Default,
        Type_Count
    };

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Property name array
    /// </summary >
    static pcstr                        sm_kapszPropertyNames[];

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Property array of default values
    /// </summary >
    static const Properties::Property   sm_kaDefaultProperties[];

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Ogre3D particle system from the ParticleFX plugin.
    /// </summary >
    Ogre::ParticleSystem*               m_pParticleSystem;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object position.
    /// </summary >
    Math::Vector3                       m_Position;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object last position recorded for this object used to calculate a direction (velocity) vector.
    /// </summary >
    Math::Vector3                       m_LastPosition;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object orientation quaternion.
    /// </summary >
    Math::Quaternion                    m_Orientation;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object scale.
    /// </summary >
    Math::Vector3                       m_Scale;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Ogre3D particle system material string ID name.
    /// (This value must be unique from all other particle system objects in the scene).
    /// </summary >
    std::string                         m_strPSystemName;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Ogre3D particle system .particle source script 
    /// (This is the script name not the file name).
    /// </summary >
    std::string                         m_strPSystemSource;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object denotes special handling for the particle system.
    /// E.g. Type_FireHose receives special updates from the Input system, and camera positioning and 
    /// orientation updates whereas Type_Default receives position updates only from whatever object it tracks
    /// from the .cdf link section.
    /// </summary >
    ParticleSystemType                  m_PSystemType;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object Ogre3D particle system emission rate as read from the script file.
    /// </summary >
    Ogre::Real                          m_emissionRate;

    /// <summary >
    /// OGREGraphicsObjectParticleSystem object specifies whether or not to emit particles.
    /// (This does not destroy the particle system, but rather sets the emission rate to 0.
    /// </summary >
    Bool                                m_bEmitParticles;
};

