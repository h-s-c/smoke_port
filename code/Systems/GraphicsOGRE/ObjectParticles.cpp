#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"

#include <Ogre.h>

#include "Systems/GraphicsOGRE/Extras/PagedGeometry.hpp"
#include "Systems/GraphicsOGRE/Extras/GrassLoader.hpp"
#include "Systems/GraphicsOGRE/Extras/DynamicBuffer.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectParticles.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


#define POGRESCENEMGR (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRESceneManager())
#define PSYSTEM         (reinterpret_cast<OGREGraphicsSystem*>(m_pSystemScene->GetSystem()))

#define WATER_PARTICLE 7

const f32 OGREGraphicsObjectParticles::m_fFireBBHeight = 500.0f;

u32 OGREGraphicsObjectParticles::sm_EntityId = 0;

pcstr OGREGraphicsObjectParticles::sm_kapszPropertyNames[] =
{
    "PointList", "ProceduralPointList", "Material", "CastShadows",
};

const Properties::Property OGREGraphicsObjectParticles::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_PointList ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_ProceduralPointList ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Material ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::WriteOnly,
                          nullptr, nullptr, nullptr, nullptr,
                          "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_CastShadows ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          0 ),
};


OGREGraphicsObjectParticles::OGREGraphicsObjectParticles(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : OGREGraphicsObject( pSystemScene, pszName )
    , m_pDynamicObject( nullptr )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Type = OGREGraphicsObject::Type_PointList;
}


OGREGraphicsObjectParticles::~OGREGraphicsObjectParticles(
    void
    )
{
    if( m_pDynamicObject )
    {
        m_pNode->detachObject( m_pDynamicObject );
    }
    SAFE_DELETE(m_pDynamicObject);
}


Error
OGREGraphicsObjectParticles::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err = Errors::Success;

    ASSERT( !m_bInitialized );

    //
    // Call the base class.
    //
    Err = OGREGraphicsObject::Initialize( Properties );

    //
    // Read in the initialization only properties.
    //

    for (auto & property : Properties)
    {
        if ( property.GetFlags() & Properties::Flags::Valid &&
             property.GetFlags() & Properties::Flags::InitOnly )
        {
            std::string sName = property.GetName();

            if ( sName == sm_kapszPropertyNames[ Property_PointList ] )
            {
                property.ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_ProceduralPointList ] )
            {
                 property.ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    m_pDynamicObject = new DynamicBuffer();
    if (!m_pDynamicObject) {
        ASSERT( False );
        Err = Errors::Failure;
    }
    m_pDynamicObject->setCustomParameter( 0, Ogre::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
    m_pDynamicObject->setCustomParameter( 1, Ogre::Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    m_pNode->attachObject( m_pDynamicObject );

    //
    // Set the remaining properties for this object.
    //
    SetProperties( Properties );

    return Err;
}


void
OGREGraphicsObjectParticles::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for (auto & sm_kaDefaultPropertie : sm_kaDefaultProperties)
    {
        Properties.push_back( sm_kaDefaultPropertie );
    }
}


void
OGREGraphicsObjectParticles::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for (auto & property : Properties)
    {
        if ( property.GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = property.GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Material ] )
            {
                if ( m_pDynamicObject != nullptr )
                {
                    m_pDynamicObject->setMaterial( property.GetStringPtr( 0 ) );
                }
                else
                {
                    ASSERT( False );
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_CastShadows ] )
            {
                if ( m_pDynamicObject != nullptr )
                {
                    m_pDynamicObject->setCastShadows( property.GetBool( 0 ) != False );
                }
                else
                {
                    ASSERT( False );
                }
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            property.ClearFlag( Properties::Flags::Valid );
        }
    }
}


System::Types::BitMask
OGREGraphicsObjectParticles::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Custom | System::Changes::Graphics::All | OGREGraphicsObject::GetDesiredSystemChanges();
}


Error
OGREGraphicsObjectParticles::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);

    std::lock_guard<std::mutex> lock(m_mutex);

    if ( ChangeType & System::Changes::Graphics::VertexBuffer )
    {
        //
        // Get the vertex buffer from the graphics object.
        //
        void* pVB = nullptr;
        u32 vertexCount = pGfxObj->GetVertexCount();
        m_pDynamicObject->prepareHardwareBuffers(vertexCount);
        pVB = m_pDynamicObject->lockBuffer();
        {
            pGfxObj->GetVertices( pVB );
        }
        m_pDynamicObject->unlockBuffer();

        //
        // Get the AABB information from the graphics object.
        //
        Base::Vector3 AABBMin;
        Base::Vector3 AABBMax;
        pGfxObj->GetAABB( AABBMin, AABBMax );
        AABBMax.y += m_fFireBBHeight;  // make bounding boxes a taller for the embers and smoke
        Ogre::AxisAlignedBox oaabb(Ogre::Vector3(AABBMin.x,AABBMin.y,AABBMin.z),Ogre::Vector3(AABBMax.x,AABBMax.y,AABBMax.z));
        m_pDynamicObject->setBoundingBox(oaabb);

        m_pDynamicObject->setExtents( AABBMin, AABBMax );
        
        m_pNode->_updateBounds(); // Update the "dirty" flag in Ogre for the node so that the new bounding box is rendered!

        f32 Height = static_cast<f32>(PSYSTEM->GetOGRERenderWindow()->getHeight());

        m_pDynamicObject->setCustomParameter( 1, Ogre::Vector4( Height, 0.0f, 0.0f, 0.0f ) );

        m_pDynamicObject->setVisible( true );
        m_pNode->setVisible( true );
    }

    if ( ChangeType & System::Changes::Graphics::AABB )
    {
        //
        // Get the AABB information from the graphics object.
        //
        Base::Vector3 AABBMin;
        Base::Vector3 AABBMax;
        pGfxObj->GetAABB( AABBMin, AABBMax );

        m_pDynamicObject->setExtents( AABBMin, AABBMax );
        Ogre::AxisAlignedBox oaabb(Ogre::Vector3(AABBMin.x,AABBMin.y,AABBMin.z),Ogre::Vector3(AABBMax.x,AABBMax.y,AABBMax.z));
        m_pDynamicObject->setBoundingBox(oaabb);
    }

    if ( ChangeType & System::Changes::Graphics::VertexDecl )
    {
        //
        // Get the vertex information from the graphics object.
        //
        IGraphicsParticleObject* pParticleObject = dynamic_cast<IGraphicsParticleObject*>(pSubject);

        u32 VertexDeclCount = pGfxObj->GetVertexDeclarationCount();
        auto  pVertexDecl = new VertexDecl::Element[ VertexDeclCount ];
        ASSERT( pVertexDecl != NULL );
        pGfxObj->GetVertexDeclaration( pVertexDecl );

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
            Ogre::VES_BLEND_INDICES,
            Ogre::VES_BLEND_WEIGHTS,
        };

        std::string szTechniqueName;
        size_t Offset = 0;
        m_pOgreVertexDecl = m_pDynamicObject->getVertexDeclaration();

        m_pOgreVertexDecl->removeAllElements();
        u16 texUsageCount = 0;
        for ( u32 i=0; i < VertexDeclCount; i++ )
        {
            Offset += m_pOgreVertexDecl->addElement(
                0, Offset, aVETs[ pVertexDecl[ i ].Type ],
                aVESs[ pVertexDecl[ i ].Usage ],
                ( ( pVertexDecl[ i ].Usage == VertexDecl::Usage::Texture ) ? texUsageCount++ : 0 )
                ).getSize();
        }

        //
        // Set the Material type
        //
        szTechniqueName = pParticleObject->GetParticleGroupTechnique();
        m_pDynamicObject->setMaterial( szTechniqueName );

        delete[] pVertexDecl;

        //---------------------------------------------------------------------
        // Give the dynamic objects their own rendering queue towards the end.
        // This makes them draw after other transparent objects in the scene,
        // like the house windows
        //
        m_pDynamicObject->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);

        //---------------------------------------------------------------------
        // Here we skew the draw order of the smoke and embers particles. The
        // smoke will draw first, then the fire (at 0), finally the embers
        //
        if (!szTechniqueName.compare("FirePatchSmoke"))
        {
            m_pDynamicObject->setDistanceFromCamera(50);
        }
        else if (!szTechniqueName.compare("FirePatchEmbers"))
        {
            m_pDynamicObject->setDistanceFromCamera(-50);
        }
    }

    return OGREGraphicsObject::ChangeOccurred( pSubject, ChangeType );
}


System::Changes::BitMask
OGREGraphicsObjectParticles::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Graphics::All | OGREGraphicsObject::GetPotentialSystemChanges();
}


u32
OGREGraphicsObjectParticles::GetVertexDeclarationCount(
    In  i32 BufferBindingIndex
    )
{
    UNREFERENCED_PARAM( BufferBindingIndex );
    ASSERTMSG( False, "Need to implement this." );
    return 0;
}


void
OGREGraphicsObjectParticles::GetVertexDeclaration(
    Out VertexDecl::Element* pVertexDecl,
    In  i32 BufferBindingIndex
    )
{
    UNREFERENCED_PARAM( pVertexDecl );
    UNREFERENCED_PARAM( BufferBindingIndex );
    ASSERTMSG( False, "Need to implement this." );
    return;
}


u32
OGREGraphicsObjectParticles::GetVertexCount(
    void
    )
{
    ASSERTMSG( False, "Need to implement this." );
    return 0;
}



void
OGREGraphicsObjectParticles::GetVertices(
    Out void* pVertices
    )
{
    UNREFERENCED_PARAM( pVertices );
    ASSERTMSG( False, "Need to implement this." );
    return;
}


void
OGREGraphicsObjectParticles::GetAABB(
    Out Base::Vector3& Min,
    Out Base::Vector3& Max
    )
{
    UNREFERENCED_PARAM( Min );
    UNREFERENCED_PARAM( Max );
    ASSERTMSG( False, "Need to implement this." );
    return;

}


u32
OGREGraphicsObjectParticles::GetParticleGroupCount(
    void
    )
{
    ASSERTMSG( False, "Need to implement this." );
    return 0;
}


void
OGREGraphicsObjectParticles::GetParticleGroup(
    u32 iParticleGroup,
    Out ParticleGroupData& ParticleGroup
    )
{
    DBG_UNREFERENCED_PARAM( iParticleGroup );
    DBG_UNREFERENCED_PARAM( ParticleGroup );

    ASSERTMSG( False, "Need to implement this." );
}


u32
OGREGraphicsObjectParticles::GetParticleCount(
    u32 iParticleGroup
    )
{
    DBG_UNREFERENCED_PARAM( iParticleGroup );

    ASSERTMSG( False, "Need to implement this." );
    return 0;
}

std::string
OGREGraphicsObjectParticles::GetParticleGroupTechnique(
    void
    )
{
    ASSERTMSG( False, "Need to implement this." );
    return "No_Technique";
}

void
OGREGraphicsObjectParticles::GetParticles(
    u32 iParticleGroup,
    Out ParticleData* pParticles
    )
{
    DBG_UNREFERENCED_PARAM( iParticleGroup );
    DBG_UNREFERENCED_PARAM( pParticles );

    ASSERTMSG( False, "Need to implement this." );
    return;
}

