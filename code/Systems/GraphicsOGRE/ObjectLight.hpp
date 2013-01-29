#pragma once

class OGREGraphicsSystem;
class OGREGraphicsScene;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   A class to wrap Ogre light functionality.
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class OGREGraphicsObjectLight : public OGREGraphicsObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;


protected:

    OGREGraphicsObjectLight( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectLight( void );

    /// <summary cref="OGREGraphicsObjectLight::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectLight::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectLight::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );


protected:

    enum LightTypes
    {
        LightType_Invalid=-1,
        LightType_Point, LightType_Spot,
        LightType_Count,
    };

    static const pcstr                  sm_kapszLightTypeEnumOptions[];

    LightTypes                          m_LightType;

    enum PropertyTypes
    {
        Property_Type, Property_Position,
        Property_Diffuse, Property_Specular,
        Property_Direction, Property_Range, Property_Attenuation,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Ogre::Light*                        m_pLight;
};

