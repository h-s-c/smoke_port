#pragma once


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   A class that extends OGREGraphicsObjectLight to simulate a flicking light.
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class OGREGraphicsObjectLightFire : public OGREGraphicsObjectLight
{

public:

    OGREGraphicsObjectLightFire( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectLightFire( void );

    /// <summary cref="OGREGraphicsObjectLightFire::Update">
    ///   Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="OGREGraphicsObject::Update"/>
    virtual void Update( f32 DeltaTime );

protected:

    /// <summary cref="OGREGraphicsObjectLightFire::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObjectLight::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectLightFire::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObjectLight::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectLightFire::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObjectLight::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectLightFire::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectLightFire::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

protected:

    enum PropertyTypes
    {
        Property_Flare,
        Property_Flicker,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Ogre::ColourValue m_BaseColor;
    Ogre::ColourValue m_BaseFlare;
    Ogre::ColourValue m_Flare;
    f32               m_Flicker;
    f32               m_TotalTime;
};

