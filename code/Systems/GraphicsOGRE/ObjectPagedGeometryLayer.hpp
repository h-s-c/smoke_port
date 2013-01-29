#pragma once

class OGREGraphicsSystem;
class OGREGraphicsScene;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This describes a layer of the paged geometry system. The paged geometry system can
/// support multiple instances of itself with multiple layers per instance. We limit
/// ourselves to one instance tied to the grass loader. Multiple layers can be added
/// which would allow for a layer of grass and a layer of flowers for example.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class OGREGraphicsObjectLayer : public OGREGraphicsObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;


protected:

    OGREGraphicsObjectLayer( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectLayer( void );

    /// <summary cref="OGREGraphicsObjectLayer::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectLayer::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectLayer::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );


protected:

    enum PropertyTypes
    {
        Property_MinimumSize, Property_MaximumSize,
        Property_Animation, Property_Density,
        Property_MapBounds, Property_MaterialName,
        Property_Color, Property_RenderTechnique,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Forests::GrassLayer                *m_pLayer;
};
