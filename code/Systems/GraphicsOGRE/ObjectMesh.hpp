#pragma once


class OGREGraphicsSystem;
class OGREGraphicsScene;
class OGREGraphicsTask;
class DynamicLines;
class NormalizedLines;

/*******************************************************************************
* CLASS: OGREGraphicsObjectMesh
*
* DESCRIPTION:
* Implementation of the IGraphicsObject interface.
* See Interfaces\Graphics.h and Interfaces\System.h for a definition of the
*  class and its functions.
*******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   A class that extends OGREGraphicsObject for mesh objects.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class OGREGraphicsObjectMesh : public OGREGraphicsObject, public IGraphicsObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;
    friend OGREGraphicsTask;



protected:

    OGREGraphicsObjectMesh( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectMesh( void );

    /// <summary cref="OGREGraphicsObjectMesh::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectMesh::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectMesh::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectMesh::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectMesh::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="OGREGraphicsObjectMesh::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectMesh::GetSubMeshCount">
    ///   Implementation of the IGraphicsObject GetSubMeshCount function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetSubMeshCount"/>
    virtual u32 GetSubMeshCount( void );

    /// <summary cref="OGREGraphicsObjectMesh::GetIndexDeclaration">
    ///   Implementation of the IGraphicsObject GetIndexDeclaration function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetIndexDeclaration"/>
    virtual u32 GetIndexDeclaration( In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetVertexDeclarationCount">
    ///   Implementation of the IGraphicsObject GetVertexDeclarationCount function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexDeclarationCount"/>
    virtual u32 GetVertexDeclarationCount( In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetVertexDeclaration">
    ///   Implementation of the IGraphicsObject GetVertexDeclaration function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexDeclaration"/>
    virtual void GetVertexDeclaration( Out VertexDecl::Element* pVertexDecl, 
                                       In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetIndexCount">
    ///   Implementation of the IGraphicsObject GetIndexCount function.
    /// </summary>
    /// <seealso cref="OGREGraphicsObjectMesh::GetIndexCount"/>
    virtual u32 GetIndexCount( In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetVertexCount">
    ///   Implementation of the IGraphicsObject GetVertexCount function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexCount"/>
    virtual u32 GetVertexCount( In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetIndices">
    ///   Implementation of the IGraphicsObject GetIndices function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetIndices"/>
    virtual void GetIndices( Out void* pIndices,
                             In  u16 nSubMeshIndex = 0 );

    /// <summary cref="OGREGraphicsObjectMesh::GetVertices">
    ///   Implementation of the IGraphicsObject GetVertices function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertices"/>
    virtual void GetVertices( Out void* pVertices,
                              In  u16 nSubMeshIndex = 0,
                              In  u16 nStreamIndex = 0,
                              In  u32 nVertexDeclCount = 0,
                              In  VertexDecl::Element* pVertexDecl = NULL );

    /// <summary cref="OGREGraphicsObjectMesh::GetStreamsChanged">
    ///   Implementation of the IGraphicsObject GetStreamsChanged function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetStreamsChanged"/>
    virtual u32 GetStreamsChanged( void );

    /// <summary cref="OGREGraphicsObjectMesh::GetAABB">
    ///   Implementation of the IGraphicsObject GetAABB function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetAABB"/>
    virtual void GetAABB( Out Base::Vector3& Min, Out Base::Vector3& Max );

    /// <summary cref="OGREGraphicsObjectMesh::SetupNormalsAndTangentsDisplay">
    ///   Generate data need to display normals and tangets for this mesh.
    /// </summary>
    void SetupNormalsAndTangentsDisplay( void );

    /// <summary cref="OGREGraphicsObjectMesh::SetupCaptions">
    ///   Generate data need to display caption (names) for this mesh.
    /// </summary>
    void SetupCaptions( void );

private:
    template<class IdxType> void BuildNormalsTemplate( u32 nSubMesh );

    /// <summary cref="OGREGraphicsObjectMesh::GeometryChanged">
    ///   Process a geometry (position, scale, orientation) change (called from ChangeOccurred).
    /// </summary>
    /// <seealso cref="OGREGraphicsObjectMesh::ChangeOccurred"/>
    void GeometryChanged( System::Changes::BitMask ChangeType, IGeometryObject* pGeometryObject );

protected:

    static u32                          sm_EntityId;

    enum PropertyTypes
    {
        Property_Mesh, Property_ProceduralMesh, Property_Material, 
        Property_CastShadows, Property_DrawBoundingBox, 
        Property_ShowNormals, Property_ShowTangents,
        Property_StaticGeom, Property_Instance,
        Property_Position, Property_Orientation, Property_Scale,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Base::Vector3                       m_Position;
    Base::Quaternion                    m_Orientation;
    Base::Vector3                       m_Scale;

    Ogre::Entity*                       m_pEntity;

    std::vector<NormalizedLines*>       m_pNormals;
    std::vector<NormalizedLines*>       m_pTangents;

    Ogre::MovableText*                  m_pCaption;

    std::vector<Ogre::String>           m_MaterialNames;

    std::string                         m_strStaticGrpName;
    // Index of the InstancedGeometry Object having this entity
    u32                                 m_InstancedGeomIdx;
    // Index of this entity in the list of entities in its InstancedGeom object 
    u32                                 m_ObjectIdxinInstGeom;


    // Indicates whether the Instanced Geometry object should update its contents
    // for a change in position, orientation or scale of any of its parts.
    bool                                m_Dirty;

    std::string                     sMaterialName;
    bool                            bCastShadows;
    Ogre::MeshPtr                   pMesh;    

    // Bitmask indicating vertex streams in use. Max of 32 streams.
    u32                                 m_StreamMask;

    bool                                isProcedural;
};


