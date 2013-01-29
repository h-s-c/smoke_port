//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/DynamicLines.hpp"
#include "Systems/GraphicsOGRE/MovableText.hpp"
#include "Systems/GraphicsOGRE/NormalizedLines.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectMesh.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


#define POGRESCENEMGR (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRESceneManager())


u32 OGREGraphicsObjectMesh::sm_EntityId = 0;


pcstr OGREGraphicsObjectMesh::sm_kapszPropertyNames[] =
{
    "Mesh", "ProceduralMesh", "Material", "CastShadows", 
    "DrawBoundingBox", "ShowNormals", "ShowTangents",
    "StaticGeom", "Instance",
    "Position", "Orientation", "Scale"
};

const Properties::Property OGREGraphicsObjectMesh::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Mesh ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_ProceduralMesh ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Material ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::WriteOnly,
                          NULL, NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_CastShadows ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_DrawBoundingBox ],
                          Properties::Values::Boolean,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_ShowNormals ],
                          Properties::Values::Boolean,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_ShowTangents ],
                          Properties::Values::Boolean,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_StaticGeom ],
                          Properties::Values::String,
                          Properties::Flags::Valid  | Properties::Flags::InitOnly,
                          "Group", NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Instance ],
                          Properties::Values::String,
                          Properties::Flags::Valid,
                          "Group", NULL, NULL, NULL,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Position ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          Math::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Orientation ],
                          Properties::Values::Quaternion,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          Math::Quaternion::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Scale ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          Math::Vector3::Zero ),
};


/*
 * Some VertexDecl conversion functions to and from OGRE
 */

inline Ogre::VertexElementSemantic toOgreVertexSemantic ( u32 usage )
{
    switch ( usage )
    {
        case VertexDecl::Usage::Position:
            return Ogre::VES_POSITION;

        case VertexDecl::Usage::Normal:
            return Ogre::VES_NORMAL;

        case VertexDecl::Usage::Diffuse:
            return Ogre::VES_DIFFUSE;

        case VertexDecl::Usage::Specular:
            return Ogre::VES_SPECULAR;

        case VertexDecl::Usage::Texture:
            return Ogre::VES_TEXTURE_COORDINATES;

        case VertexDecl::Usage::Tangent:
            return Ogre::VES_TANGENT;

        case VertexDecl::Usage::BlendIndices:
            return Ogre::VES_BLEND_INDICES;

        case VertexDecl::Usage::BlendWeights:
            return Ogre::VES_BLEND_WEIGHTS;

        default:
            ASSERTMSG( False, "Unhandled case statement." );
            return (Ogre::VertexElementSemantic)(0xbaadf00d);
    }
}


inline u32 fromOgreVertexSemantic ( u32 ogreVertexSemantic )
{
    switch ( ogreVertexSemantic )
    {
        case Ogre::VES_POSITION:
            return VertexDecl::Usage::Position;

        case Ogre::VES_NORMAL:
            return VertexDecl::Usage::Normal;

        case Ogre::VES_DIFFUSE:
            return VertexDecl::Usage::Diffuse;

        case Ogre::VES_SPECULAR:
            return VertexDecl::Usage::Specular;

        case Ogre::VES_TEXTURE_COORDINATES:
            return VertexDecl::Usage::Texture;

        case Ogre::VES_TANGENT:
            return VertexDecl::Usage::Tangent;

        case Ogre::VES_BLEND_INDICES:
            return VertexDecl::Usage::BlendIndices;

        case Ogre::VES_BLEND_WEIGHTS:
            return VertexDecl::Usage::BlendWeights;

        default:
            ASSERTMSG( False, "Unhandled case statement." );
            return 0xbaadf00d;
    }
}


inline Ogre::VertexElementType toOgreVertexType ( u32 type )
{
    switch ( type )
    {
        case VertexDecl::Type::Color:
            return Ogre::VET_COLOUR;

        case VertexDecl::Type::Float1:
            return Ogre::VET_FLOAT1;

        case VertexDecl::Type::Float2:
            return Ogre::VET_FLOAT2;

        case VertexDecl::Type::Float3:
            return Ogre::VET_FLOAT3;

        case VertexDecl::Type::Float4:
            return Ogre::VET_FLOAT4;

        case VertexDecl::Type::UByte4:
            return Ogre::VET_UBYTE4;

        default:
            ASSERTMSG( False, "Unhandled case statement." );
            return (Ogre::VertexElementType)(0xbaadf00d);
    }
}


inline u32 fromOgreVertexType ( u32 ogreVertexType )
{
    switch ( ogreVertexType )
    {
        case Ogre::VET_COLOUR:
        case Ogre::VET_COLOUR_ARGB:
        case Ogre::VET_COLOUR_ABGR:
            return VertexDecl::Type::Color;

        case Ogre::VET_FLOAT1:
            return VertexDecl::Type::Float1;

        case Ogre::VET_FLOAT2:
            return VertexDecl::Type::Float2;

        case Ogre::VET_FLOAT3:
            return VertexDecl::Type::Float3;

        case Ogre::VET_FLOAT4:
            return VertexDecl::Type::Float4;

        case Ogre::VET_UBYTE4:
            return VertexDecl::Type::UByte4;

        default:
            ASSERTMSG( False, "Unhandled case statement." );
            return 0xbaadf00d;
    }
}


OGREGraphicsObjectMesh::OGREGraphicsObjectMesh(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : OGREGraphicsObject( pSystemScene, pszName )
    , m_Position( Math::Vector3::Zero )
    , m_Orientation( Math::Quaternion::Zero )
    , m_Scale( Math::Vector3::One )
    , m_pEntity( NULL )
    , m_pCaption( NULL )
    , m_strStaticGrpName ( "" )
    , m_Dirty( True )    // Force Instanced Geom update initially
    , isProcedural( false )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Type = OGREGraphicsObject::Type_Mesh;
}


OGREGraphicsObjectMesh::~OGREGraphicsObjectMesh(
    void
    )
{
    if ( m_pEntity != NULL )
    {
        m_pNode->detachObject( m_pEntity );
        POGRESCENEMGR->destroyEntity( m_pEntity );
    }

    if ( isProcedural && !pMesh.isNull() )
    {
        Ogre::MeshManager::getSingleton().remove( pMesh->getName() );
    }

    std::vector< NormalizedLines* >::iterator  it;

    for( it = m_pNormals.begin(); it != m_pNormals.end(); it++)
    {
        SAFE_DELETE( *it );
    }

    for( it = m_pTangents.begin(); it != m_pTangents.end(); it++)
    {
        SAFE_DELETE( *it );
    }

    SAFE_DELETE( m_pCaption );
}


Error
OGREGraphicsObjectMesh::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err = Errors::Failure;

    ASSERT( !m_bInitialized );

    //
    // Call the base class.
    //
    OGREGraphicsObject::Initialize( Properties );

    //
    // Read in the initialization only properties.
    //
    pcstr pszMeshName = NULL;

    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid &&
             it->GetFlags() & Properties::Flags::InitOnly )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Mesh ] )
            {
                pszMeshName = it->GetStringPtr( 0 );

                PostChanges( System::Changes::Graphics::AllMesh |
                    System::Changes::Custom);
            }
            else if ( sName == sm_kapszPropertyNames[ Property_ProceduralMesh ] )
            {
                isProcedural = true;
            }
            else if ( sName == sm_kapszPropertyNames[ Property_StaticGeom ] )
            {
                m_strStaticGrpName = it->GetString( 0 );
            }
            else
            {
                ASSERT( False );
            }
            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }

    ASSERT( pszMeshName != NULL  ||  isProcedural );

    if ( pszMeshName != NULL )
    {
        //
        // Use the UObject's name for this entity.
        //
        m_pEntity = POGRESCENEMGR->createEntity( m_pszName, pszMeshName );
        ASSERT( m_pEntity != NULL );

        m_pNode->attachObject( m_pEntity );
    }

    if ( m_pEntity != NULL  ||  isProcedural )
    {
        //
        // Set this set as initialized.
        //
        m_bInitialized = True;

        //
        // Set the remaining properties for this object.
        //
        SetProperties( Properties );

        Err = Errors::Success;
    }

    return Err;
}


void
OGREGraphicsObjectMesh::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }

    //
    // Modify the default values.
    //
    Properties[ iProperty+Property_Position ].SetValue( m_Position );
    Properties[ iProperty+Property_Orientation ].SetValue( m_Orientation );
    Properties[ iProperty+Property_Scale ].SetValue( m_Scale );

    if ( m_pEntity != NULL )
    {
        Properties[ iProperty+Property_Mesh ].SetValue( 0, m_pEntity->getMesh()->getName() );

        Properties[ iProperty+Property_CastShadows ].SetValue( 0, m_pEntity->getCastShadows() );
    }
    else if ( isProcedural )
    {
        Properties[ iProperty+Property_Mesh ].SetValue(
            0, sMaterialName.c_str()
            );

        Properties[ iProperty+Property_CastShadows ].SetValue(
            0, bCastShadows
            );
    }
}


void
OGREGraphicsObjectMesh::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Material ] )
            {
                if ( m_pEntity != NULL )
                {
                    m_pEntity->setMaterialName( it->GetStringPtr( 0 ) );
                }
                else if ( isProcedural )
                {
                    sMaterialName = it->GetStringPtr( 0 );
                }
                else
                {
                    ASSERT( False );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_CastShadows ] )
            {
                if ( m_pEntity != NULL )
                {
                    m_pEntity->setCastShadows( it->GetBool( 0 ) != False );
                }
                else if ( isProcedural )
                {
                    bCastShadows = it->GetBool( 0 ) != False;
                }
                else
                {
                    ASSERT( False );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Position ] )
            {
                m_Position = it->GetVector3();
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Orientation ] )
            {
                m_Orientation = it->GetQuaternion();
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Scale ] )
            {
                m_Scale = it->GetVector3();
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}


System::Types::BitMask
OGREGraphicsObjectMesh::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Custom | System::Changes::Graphics::All | OGREGraphicsObject::GetDesiredSystemChanges();
}


namespace {

    // Provides access to protected members of the Ogre::SceneNode class
    // in order to check if the locking is necessary
    class AccessOgreSceneNode : public Ogre::SceneNode
    {
        // Never use this class
        AccessOgreSceneNode ( const AccessOgreSceneNode& );
        AccessOgreSceneNode ();
        const AccessOgreSceneNode& operator= ( const AccessOgreSceneNode& );
    public:

        bool NeedsLocking ()
        {
            return mParent && !mParentNotified;
        }
    };

    inline 
    bool NeedsLocking ( Ogre::SceneNode* pNode )
    {
        return reinterpret_cast<AccessOgreSceneNode*>(pNode)->NeedsLocking();
    }

} // anonymous namespace


Error
OGREGraphicsObjectMesh::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    // THREAD SAFETY NOTE
    // Currently some of the notifications do not arrive concurrently.
    // If they ever do, calls marked as "Lock it" must be protected by locks.


    if ( ChangeType & (System::Changes::Graphics::IndexDecl |
                       System::Changes::Graphics::VertexDecl) )
    {
        //
        // Get the vertex information from the graphics object.
        //
        IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);
        ASSERT( pGfxObj != NULL );

        if( pGfxObj )
        {
            u32 IndexDecl = pGfxObj->GetIndexDeclaration();
            u32 VertexDeclCount = pGfxObj->GetVertexDeclarationCount();
            VertexDecl::Element* pVertexDecl = new VertexDecl::Element[ VertexDeclCount ];
            ASSERT( pVertexDecl != NULL );
            pGfxObj->GetVertexDeclaration( pVertexDecl );

            u32 IndexCount = pGfxObj->GetIndexCount();
            u32 VertexCount = pGfxObj->GetVertexCount();

            //
            // Get rid of the old mesh and entity.
            //
            if ( !pMesh.isNull() )
            {
                m_pNode->detachObject( m_pEntity );
                POGRESCENEMGR->destroyEntity( m_pEntity );
                Ogre::MeshManager::getSingleton().remove( pMesh->getName() );

                //
                // Currently don't allow redoing a mesh.
                //
                ASSERT( False );
            }

            //
            // Create a unique mesh name.
            //
            char szMeshName[ 256 ];
            sprintf_s( szMeshName, sizeof szMeshName, "%s_ProceduralMesh", m_pszName );

            //
            // Create the mesh and an accompanying sub-mesh.
            //
            pMesh =
                Ogre::MeshManager::getSingleton().createManual( szMeshName, "ProceduralMeshes" );

            Ogre::SubMesh* pSubMesh = pMesh->createSubMesh(); // Lock it, see THREAD SAFETY NOTE above

            //
            // Create the vertex declaration using the mappings below to go from those defined in
            //  Interfaces/Graphics.h to those that Ogre uses.
            //
            static const Ogre::VertexElementType aVETs[] =
            {
                Ogre::VET_COLOUR, Ogre::VET_FLOAT1, Ogre::VET_FLOAT2, Ogre::VET_FLOAT3, Ogre::VET_FLOAT4
            };

            static const Ogre::VertexElementSemantic aVESs[] =
            {
                Ogre::VES_POSITION, 
                Ogre::VES_NORMAL, 
                Ogre::VES_DIFFUSE, 
                Ogre::VES_SPECULAR,
                Ogre::VES_TANGENT,
                Ogre::VES_TEXTURE_COORDINATES,
                Ogre::VES_TEXTURE_COORDINATES,
                Ogre::VES_TEXTURE_COORDINATES,
                Ogre::VES_TEXTURE_COORDINATES,
            };

            pSubMesh->useSharedVertices = false;
            pSubMesh->vertexData = new Ogre::VertexData();
            pSubMesh->vertexData->vertexCount = VertexCount;

            Ogre::VertexDeclaration* pOgreVertexDecl = pSubMesh->vertexData->vertexDeclaration;

            u32 cVertexDeclElements = pGfxObj->GetVertexDeclarationCount();
            size_t Offset[32] = {0};
            m_StreamMask = 0;
            for ( u32 i=0; i < cVertexDeclElements; i++ ) // Lock it, see THREAD SAFETY NOTE above
            {
                u32 stream = pVertexDecl[ i ].StreamIndex;

                Offset[ stream ] += pOgreVertexDecl->addElement(
                    stream, 
                    Offset[ stream ], 
                    aVETs[ pVertexDecl[ i ].Type ],
                    aVESs[ pVertexDecl[ i ].Usage ]
                ).getSize();

                m_StreamMask |= ( 1 << stream );
            }

            for( u32 i = 0; i < 32; i++ )
            {
                if( m_StreamMask & (1<<i) )
                {
                    //
                    // Create the vertex buffer for each stream.
                    //
                    Ogre::HardwareVertexBufferSharedPtr pVertexBuffer =
                        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                        pOgreVertexDecl->getVertexSize( i ),
                        VertexCount,
                        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
                        );

                    pSubMesh->vertexData->vertexBufferBinding->setBinding( i, pVertexBuffer );
                }
            }

            //
            // Create the index buffer.
            //
            Ogre::HardwareIndexBuffer::IndexType IndexType = (IndexDecl == IndexDecl::Type::Index16) ?
                Ogre::HardwareIndexBuffer::IT_16BIT :
            Ogre::HardwareIndexBuffer::IT_32BIT;

            pSubMesh->indexData->indexStart = 0;
            pSubMesh->indexData->indexCount = IndexCount;
            pSubMesh->indexData->indexBuffer =
                Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                IndexType, IndexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
                );

            //
            // Load the mesh.
            //
            pMesh->load();

            //
            // Create the entity.
            //
            m_pEntity = POGRESCENEMGR->createEntity( m_pszName, szMeshName ); //Lock it, see THREAD SAFETY NOTE above
            ASSERT( m_pEntity != NULL );

            if ( m_pEntity != NULL )
            {
                m_pEntity->setMaterialName( sMaterialName );
                m_pEntity->setCastShadows( bCastShadows );
            }

            m_pNode->attachObject( m_pEntity );
        }    
    }

    if ( ChangeType & System::Changes::Graphics::AABB ) // Lock it, see THREAD SAFETY NOTE above
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        ASSERTMSG( !pMesh.isNull(),
                   "The index/vertex decl needs to happen before or at the same time as this." );

        //
        // Get the AABB information from the graphics object.
        //
        IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);
        ASSERT( pGfxObj != NULL );

        Math::Vector3 AABBMin;
        Math::Vector3 AABBMax;
        pGfxObj->GetAABB( AABBMin, AABBMax );


        //
        // Set the mesh's bounding box.
        //
        Ogre::AxisAlignedBox AABox;
        AABox.setMaximum( Ogre::Vector3( AABBMax.x, AABBMax.y, AABBMax.z ) );
        AABox.setMinimum( Ogre::Vector3( AABBMin.x, AABBMin.y, AABBMin.z ) );
        pMesh->_setBounds( AABox );

        m_pNode->_updateBounds();
    }

    if ( ChangeType & System::Changes::Graphics::IndexBuffer )
    {
        ASSERTMSG( !pMesh.isNull(),
                   "The index/vertex decl needs to happen before or at the same time as this." );

        //
        // Get the index buffer from the graphics object.
        //
        IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);
        ASSERT (pGfxObj != NULL );

        u32 nSubMeshCount = pGfxObj->GetSubMeshCount();

        for( u32 i = 0; i < nSubMeshCount; i++ ) // Lock it, see THREAD SAFETY NOTE above?
        {
            Ogre::SubMesh* pSubMesh = pMesh->getSubMesh( i );
            Ogre::HardwareIndexBufferSharedPtr pIndexBuffer = pSubMesh->indexData->indexBuffer;

            void* pIB = pIndexBuffer->lock( Ogre::HardwareBuffer::HBL_DISCARD );
            pGfxObj->GetIndices( pIB, i );
            pIndexBuffer->unlock();
        }
    }

    if ( ChangeType & System::Changes::Graphics::VertexBuffer )
    {
        ASSERTMSG( !pMesh.isNull(),
                   "The index/vertex decl needs to happen before or at the same time as this." );

        //
        // Get the vertex buffer from the graphics object.
        //
        IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);
        ASSERT( pGfxObj != NULL );

        u32 nSubMeshCount = pGfxObj->GetSubMeshCount();

        // Figure out which vertex streams changed.
        u32 nStreamMask = pGfxObj->GetStreamsChanged();

        for( u32 i = 0; i < nSubMeshCount; i++ )
        {
            Ogre::SubMesh* pSubMesh = pMesh->getSubMesh( i );

            for( u32 j=0; j<32; j++ ) // Loop through vertex streams
            {
                if( nStreamMask & (1<<j) )
                {
                    Ogre::HardwareVertexBufferSharedPtr pVertexBuffer =
                        pSubMesh->vertexData->vertexBufferBinding->getBuffer( j ); //Lock it, see THREAD SAFETY NOTE above?

                    void* pVB = pVertexBuffer->lock( 0, 0, Ogre::HardwareBuffer::HBL_DISCARD );
                    pGfxObj->GetVertices( pVB, i, j );
                    pVertexBuffer->unlock();
                }
            }
        }
    }

    if ( ChangeType & (System::Changes::Geometry::Position |
                       System::Changes::Geometry::Orientation |
                       System::Changes::Geometry::Scale)
       )
    {
        IGeometryObject* pGeometryObject = dynamic_cast<IGeometryObject*>(pSubject);
        ASSERT( pGeometryObject != NULL );

        if(m_pNode)
        {
            if ( NeedsLocking(m_pNode) )
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                GeometryChanged( ChangeType, pGeometryObject );
            }
            else
            {
                GeometryChanged( ChangeType, pGeometryObject );
            }
        }
        else
        {
            if ( ChangeType & System::Changes::Geometry::Position )
            {
                m_Position = *pGeometryObject->GetPosition();
                m_Dirty = True;
            }
            if ( ChangeType & System::Changes::Geometry::Orientation )
            {
                m_Orientation = *pGeometryObject->GetOrientation();
                m_Dirty = True;
            }
            if ( ChangeType & System::Changes::Geometry::Scale )
            {
                m_Scale = *pGeometryObject->GetScale();
                m_Dirty = True;
            }
        }
    }

    return Errors::Success;
}

void OGREGraphicsObjectMesh::GeometryChanged( System::Changes::BitMask ChangeType, IGeometryObject* pGeometryObject )
{
    if ( ChangeType & System::Changes::Geometry::Position )
    {
        m_Position = *pGeometryObject->GetPosition();
        m_pNode->setPosition( m_Position.x, m_Position.y, m_Position.z );
        m_Dirty = True;
    }

    if ( ChangeType & System::Changes::Geometry::Orientation )
    {
        m_Orientation = *pGeometryObject->GetOrientation();
        m_pNode->setOrientation( m_Orientation.w, m_Orientation.x,
                                 m_Orientation.y, m_Orientation.z );
        m_Dirty = True;
    }

    if ( ChangeType & System::Changes::Geometry::Scale )
    {
        m_Scale = *pGeometryObject->GetScale();
        m_pNode->setScale(m_Scale.x, m_Scale.y, m_Scale.z );
        m_Dirty = True;
    }
} // OGREGraphicsObjectMesh::UpdateGeometry



System::Changes::BitMask
OGREGraphicsObjectMesh::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Graphics::All | OGREGraphicsObject::GetPotentialSystemChanges();
}


u32
OGREGraphicsObjectMesh::GetSubMeshCount(
    void
    )
{
    return m_pEntity->getMesh()->getNumSubMeshes();
}


u32
OGREGraphicsObjectMesh::GetIndexDeclaration(
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    return (pSubMesh->indexData->indexBuffer.getPointer()->getType() ==
            Ogre::HardwareIndexBuffer::IndexType::IT_16BIT) ?
                IndexDecl::Type::Index16 :
                IndexDecl::Type::Index32;
}


u32
OGREGraphicsObjectMesh::GetVertexDeclarationCount(
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    Ogre::VertexDeclaration* pOgreVertexDecl = NULL;  
    if(pSubMesh->useSharedVertices)
    {
        pOgreVertexDecl = m_pEntity->getMesh()->sharedVertexData->vertexDeclaration;
    }
    else
    {
        pOgreVertexDecl = pSubMesh->vertexData->vertexDeclaration;
    }

    u32 total_count = static_cast<u32>(pOgreVertexDecl->getElementCount());

    // Consider all buffer sources
    return total_count;
}


void
OGREGraphicsObjectMesh::GetVertexDeclaration(
    Out VertexDecl::Element* pVertexDecl,
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    Ogre::VertexDeclaration* pOgreVertexDecl = NULL;  
    if(pSubMesh->useSharedVertices)
    {
        pOgreVertexDecl = m_pEntity->getMesh()->sharedVertexData->vertexDeclaration;
    }
    else
    {
        pOgreVertexDecl = pSubMesh->vertexData->vertexDeclaration;
    }

    u32 Elements = static_cast<u32>(pOgreVertexDecl->getElementCount());

    for ( u32 i=0; i < Elements; i++ )
    {
        const Ogre::VertexElement& Element =
            *pOgreVertexDecl->getElement( static_cast<u16>(i) );

        pVertexDecl[ i ].Type        = fromOgreVertexType( Element.getType() );
        pVertexDecl[ i ].Usage       = fromOgreVertexSemantic( Element.getSemantic() );     
        pVertexDecl[ i ].UsageIndex  = Element.getIndex();
        pVertexDecl[ i ].StreamIndex = Element.getSource();
    }    
}


u32
OGREGraphicsObjectMesh::GetIndexCount(
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    return static_cast<u32>(pSubMesh->indexData->indexCount);
}


u32
OGREGraphicsObjectMesh::GetVertexCount(
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    u32 uVertexCount;
    if(pSubMesh->useSharedVertices)
    {
        uVertexCount = static_cast<u32>(m_pEntity->getMesh()->sharedVertexData->vertexCount);
    }
    else
    {
        uVertexCount = static_cast<u32>(pSubMesh->vertexData->vertexCount);
    }

    return uVertexCount;
}


void
OGREGraphicsObjectMesh::GetIndices(
    Out void* pIndices,
    In  u16 nSubMeshIndex
    )
{
    ASSERT( m_pEntity != NULL );

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    Ogre::HardwareIndexBufferSharedPtr pHIB = pSubMesh->indexData->indexBuffer;

    void* pIB = pHIB->lock( Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY );
    ::memcpy_s( pIndices, pHIB->getSizeInBytes(), pIB, pHIB->getSizeInBytes() );
    pHIB->unlock();
}


void
OGREGraphicsObjectMesh::GetVertices(
    Out void* pOutVertices,
    In  u16 nSubMeshIndex,
    In  u16 nStreamIndex,
    In  u32 nOutVertDeclCount,
    In  VertexDecl::Element* pOutVertDecl
    )
{
    // We don't support stream access in loaded meshes yet.
    ASSERT( nStreamIndex == 0 );
    UNREFERENCED_PARAM( nStreamIndex );

    ASSERT( m_pEntity != NULL );

    u8* pVertices = ( u8* ) pOutVertices;
    u32 nReqVertDeclCount = nOutVertDeclCount;
    VertexDecl::Element* pReqVertDecl = NULL;

    Ogre::SubMesh* pSubMesh = m_pEntity->getMesh()->getSubMesh( nSubMeshIndex );
    ASSERT( pSubMesh != NULL );

    Ogre::VertexData* pVertData = NULL;
    if(pSubMesh->useSharedVertices)
    {
        pVertData = m_pEntity->getMesh()->sharedVertexData;
    }
    else
    {
        pVertData = pSubMesh->vertexData;
    }

    u32 NumBuffers = (u32) pVertData->vertexBufferBinding->getBufferCount();
    ASSERT( 0 != NumBuffers );

    Ogre::HardwareVertexBufferSharedPtr *pHVB = new Ogre::HardwareVertexBufferSharedPtr[ NumBuffers ];
    ASSERT( pHVB != NULL );
    u8** pVB = new u8*[ NumBuffers ];
    ASSERT( pVB != NULL );

    for( u32 i = 0; i < NumBuffers; i++ )
    {        
        pHVB[i] = pVertData->vertexBufferBinding->getBuffer( i );

        pVB[i] = (u8*) (pHVB[i]->lock( Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY ));
    }

    const Ogre::VertexElement* pElement;

    if( 0 == nOutVertDeclCount )
    {
        // Return vertex list with all vertex elements for each vertex
        nReqVertDeclCount = (u32) pVertData->vertexDeclaration->getElementCount();
        pReqVertDecl = new VertexDecl::Element[ nReqVertDeclCount ];
        GetVertexDeclaration( pReqVertDecl, nSubMeshIndex );
    }
    else
    {
        pReqVertDecl = ( VertexDecl::Element* )pOutVertDecl;
    }

    for( u32 i = 0; i < pVertData->vertexCount; i++ )
    {
        for( u32 j = 0; j < nReqVertDeclCount; j++ )
        {
            pElement = pVertData->vertexDeclaration->findElementBySemantic( 
                                                        toOgreVertexSemantic( pReqVertDecl[ j ].Usage ), 
                                                        (u16) pReqVertDecl[ j ].UsageIndex );

            u16 nBufIdx = pElement->getSource();
            size_t nVertSize = pVertData->vertexDeclaration->getVertexSize( nBufIdx );

            ::memcpy_s( (void *) pVertices,
                        pElement->getSize(),
                        (void *) ( pVB[ nBufIdx ] + (i * nVertSize) + pElement->getOffset() ),
                        pElement->getSize() );

            pVertices += pElement->getSize();
        }
    }

    for( u32 i = 0; i < NumBuffers; i++ )
    {
        pHVB[i]->unlock();
    }

    if( 0 == nOutVertDeclCount )
    {
        delete[] pReqVertDecl;
    }

    if( pHVB )
    {
        delete[] pHVB;
    }

    if( pVB )
    {
        delete[] pVB;
    }
}


u32 
OGREGraphicsObjectMesh::GetStreamsChanged(
    void
    )
{
    // Always returns 1 to indicate change to stream 0 only.
    // Will have to be changed if and when we expose the Mesh streams
    // to other systems.
    return 1;
}


void
OGREGraphicsObjectMesh::GetAABB(
    Out Math::Vector3& Min,
    Out Math::Vector3& Max
    )
{
    ASSERT( m_pEntity != NULL );

    const Ogre::AxisAlignedBox& aabb = m_pEntity->getBoundingBox();

    Min.x = aabb.getMinimum().x;
    Min.y = aabb.getMinimum().y;
    Min.z = aabb.getMinimum().z;

    Max.x = aabb.getMaximum().x;
    Max.y = aabb.getMaximum().y;
    Max.z = aabb.getMaximum().z;

    ASSERT(Min.x <= Max.x && Min.y <= Max.y && Min.z <= Max.z);
}


// Template function to handle both meshes with 16-bit indices and meshes with 32-bit indices.
// The function itself is called once, when the Vertex_Normals or Vertex_Tangents display toggle 
// is used for the first time. It creates a vertex buffer containing line ticks representing
// both vertex tangents or normals. Then these are either rendered or not depending on the toggle mode.
template<class IdxType> void 
OGREGraphicsObjectMesh::BuildNormalsTemplate(
    u32 nSubMesh
    )
{
    bool bNormAvailable = true;
    bool bTangAvailable = true;

    Ogre::HardwareIndexBufferSharedPtr ibuf = m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->indexData->indexBuffer;
    Ogre::HardwareVertexBufferSharedPtr vbuf;


    IdxType* pIndices = new IdxType [ m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->indexData->indexCount ];
    void* pIB = ibuf->lock( 0, 0, Ogre::HardwareBuffer::HBL_READ_ONLY );
    ::memcpy_s( (void *)pIndices, ibuf->getSizeInBytes(), pIB, ibuf->getSizeInBytes() );
    ibuf->unlock();
   

    u32 VertexDeclCount = GetVertexDeclarationCount( nSubMesh );
    VertexDecl::Element* pVertexDecl = new VertexDecl::Element[ VertexDeclCount ];
    GetVertexDeclaration( pVertexDecl, nSubMesh );
    
    //
    // Get the location of the vertex position.
    //
    u32 Type;
    u32 uPosStream  = VertexDecl::FindUsageInStream( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Position );
    // Fail if no position information found
    ASSERT( uPosStream != static_cast<u32>(-1) );
    i32 iPosOffset  = VertexDecl::CalculateUsageOffsetInVertex(
                        pVertexDecl, VertexDeclCount, VertexDecl::Usage::Position, Type, uPosStream
                        );
    if(m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->useSharedVertices == true)
    {
        vbuf = m_pEntity->getMesh()->sharedVertexData->vertexBufferBinding->getBuffer( uPosStream );
    }
    else
    {
        vbuf = m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->vertexData->vertexBufferBinding->getBuffer( uPosStream );
    }


    u32 VertPosSize = VertexDecl::CalculateSize( pVertexDecl, VertexDeclCount, uPosStream );
    u32 VertexCount = GetVertexCount( nSubMesh );
    u8* pVertsPos = new u8[ VertexCount * VertPosSize ];

    void* pVB = vbuf->lock( Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY );
    ::memcpy_s( (void *)pVertsPos, vbuf->getSizeInBytes(), pVB, vbuf->getSizeInBytes() );
    vbuf->unlock();

    // Assume Normals and Tangents are in the same stream as Position,
    // until we find out otherwise.
    u32 VertNormSize = VertPosSize;
    u32 VertTangSize = VertPosSize;
    u8* pVertsNorm = pVertsPos;
    u8* pVertsTang = pVertsPos;


    // Really search for Normals now
    u32 uNormStream = VertexDecl::FindUsageInStream( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Normal );
    if( uNormStream == static_cast<u32>(-1) )
    {
        bNormAvailable = false;
    }

    i32 iNormOffset = -1;
    if( bNormAvailable )
    {
        iNormOffset = VertexDecl::CalculateUsageOffsetInVertex(
                            pVertexDecl, VertexDeclCount, VertexDecl::Usage::Normal, Type, uNormStream
                            );

        if( uNormStream != uPosStream )
        {
            VertNormSize = VertexDecl::CalculateSize( pVertexDecl, VertexDeclCount, uNormStream );
            pVertsNorm   = new u8[ VertexCount * VertNormSize ];

            if(m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->useSharedVertices == true)
            {
                vbuf = m_pEntity->getMesh()->sharedVertexData->vertexBufferBinding->getBuffer( uNormStream );
            }
            else
            {
                vbuf = m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->vertexData->vertexBufferBinding->getBuffer( uNormStream );
            }

            pVB = vbuf->lock( Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY );
            ::memcpy_s( (void *)pVertsNorm, vbuf->getSizeInBytes(), pVB, vbuf->getSizeInBytes() );
            vbuf->unlock();
        }
    }


    // Search for Tangents now
    i32 iTangOffset = -1;
    u32 uTangStream = VertexDecl::FindUsageInStream( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Tangent );
    if( uTangStream == static_cast<u32>(-1) )
    {
        bTangAvailable = false;
    }

    if( bTangAvailable )
    {
        iTangOffset = VertexDecl::CalculateUsageOffsetInVertex(
                            pVertexDecl, VertexDeclCount, VertexDecl::Usage::Tangent, Type, uTangStream
                            );
    }

    // If Tangent type not found, look for a 3D TexCoord, and try using that as Tangent
    u32 usageIndex = 0;
    while( !bTangAvailable )
    {
        uTangStream = VertexDecl::FindUsageInStream( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Texture, usageIndex );
        if( uTangStream == static_cast<u32>(-1) )
        {
            break;
        }

        // Let's see if this is a 3D texture
        VertexDecl::CalculateUsageOffsetInVertex( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Texture, Type, uTangStream, usageIndex );
        if( Type == VertexDecl::Type::Float3 )
        {
            bTangAvailable = true;
            break;
        }

        // Try the next set Tex Coords, if present
        usageIndex++;
    }

    if( bTangAvailable )
    {
        if( iTangOffset == -1 )
        {
            iTangOffset = VertexDecl::CalculateUsageOffsetInVertex(
                                pVertexDecl, VertexDeclCount, VertexDecl::Usage::Texture, Type, uTangStream, usageIndex
                                );
        }

        if( uTangStream != uPosStream )
        {
            VertTangSize = VertexDecl::CalculateSize( pVertexDecl, VertexDeclCount, uTangStream );
            pVertsTang   = new u8[ VertexCount * VertTangSize ];

            if(m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->useSharedVertices == true)
            {
                vbuf = m_pEntity->getMesh()->sharedVertexData->vertexBufferBinding->getBuffer( uTangStream );
            }
            else
            {
                vbuf = m_pEntity->getSubEntity(nSubMesh)->getSubMesh()->vertexData->vertexBufferBinding->getBuffer( uTangStream );
            }

            pVB = vbuf->lock( Ogre::HardwareBuffer::LockOptions::HBL_READ_ONLY );
            ::memcpy_s( (void *)pVertsTang, vbuf->getSizeInBytes(), pVB, vbuf->getSizeInBytes() );
            vbuf->unlock();
        }
    }


    if( bNormAvailable )
    {
        NormalizedLines *pNormals = new NormalizedLines("NormalizedLines_Half_Red");
        pNormals->setCastShadows(false);

        for (size_t i=0; i<ibuf->getNumIndexes(); i++) 
        {
            //
            // Get the vertex at the index.
            //
            const Math::Vector3& Vertex =
                *reinterpret_cast<const Math::Vector3*>(pVertsPos  + iPosOffset  + (pIndices[i] * VertPosSize));
            const Math::Vector3& Normal =
                *reinterpret_cast<const Math::Vector3*>(pVertsNorm + iNormOffset + (pIndices[i] * VertNormSize));

            pNormals->addPoint(Vertex, Math::Vector3::Zero);
            pNormals->addPoint(Vertex, Normal);
        }
        pNormals->update();
        m_pNormals.push_back( pNormals );
    }

    if( bTangAvailable )
    {
        NormalizedLines *pTangents = new NormalizedLines("NormalizedLines_Half_Green");
        pTangents->setCastShadows(false);
        for (size_t i=0; i<ibuf->getNumIndexes(); i++) 
        {
            const Math::Vector3& Vertex =
                *reinterpret_cast<const Math::Vector3*>(pVertsPos  + iPosOffset  + (pIndices[i]* VertPosSize));
            const Math::Vector3& Tangent =
                *reinterpret_cast<const Math::Vector3*>(pVertsTang + iTangOffset + (pIndices[i]* VertTangSize));

            pTangents->addPoint(Vertex, Math::Vector3::Zero);
            pTangents->addPoint(Vertex, Tangent);
        }
        pTangents->update();
        m_pTangents.push_back( pTangents );
    }


    // Clean up
    if( pVertsTang != pVertsPos )
    {
        SAFE_DELETE_ARRAY( pVertsTang );
    }
    if( pVertsNorm != pVertsPos )
    {
        SAFE_DELETE_ARRAY( pVertsNorm );
    }

    SAFE_DELETE_ARRAY( pVertsPos );
    SAFE_DELETE_ARRAY( pVertexDecl );
    SAFE_DELETE_ARRAY( pIndices );
}



void 
OGREGraphicsObjectMesh::SetupNormalsAndTangentsDisplay( 
    void
    )
{
    // skip Normals display for objects that got bucketed as Static or InstancedGeom
    if( m_pEntity == NULL )
    {
        return;
    }

    if( !m_pNormals.empty() || !m_pTangents.empty() )
    {
        return;
    }

    m_pNormals.clear();
    m_pTangents.clear();

    for(u32 i = 0; i < m_pEntity->getNumSubEntities(); i++ )
    {
        size_t nIndexSize = m_pEntity->getSubEntity( i )->getSubMesh()->indexData->indexBuffer->getIndexSize();

        ASSERT( (nIndexSize == 2 ) || (nIndexSize == 4 ) );
        if(nIndexSize == 2)
        {
            BuildNormalsTemplate<u16>( i );
        }
        else
        {
            BuildNormalsTemplate<u32>( i );
        }
    }
}


void
OGREGraphicsObjectMesh::SetupCaptions(
    void
    )
{
    if( m_pCaption )
    {
        return;
    }

    // Render Object names
    m_pCaption = new Ogre::MovableText( std::string(m_pszName).append("_caption"), 
                                        m_pszName, 
                                        "MyFont", 
                                        10.0f, 
                                        Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.5f) );

    // Center horizontally and display above the node
    m_pCaption->setTextAlignment(Ogre::MovableText::H_CENTER, Ogre::MovableText::V_ABOVE); 
    m_pCaption->setPolygonModeOverrideable( false );
}
