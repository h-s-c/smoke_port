// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// External
#include <Common\Base\hkBase.h>
#include <Common/Base/Types/Geometry/hkStridedVertices.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Physics\Collide\Shape\hkpShape.h>
#include <Physics\Collide\Shape\Convex\Box\hkpBoxShape.h>
#include <Physics\Collide\Shape\Convex\Sphere\hkpSphereShape.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>
#include <Physics\Collide\Shape\Deprecated\Mesh\hkpMeshShape.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppCompilerInput.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppUtility.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppBvTreeShape.h>
#include <Physics\Collide\Shape\HeightField\hkpHeightFieldShape.h>
#include <Physics\Collide\Shape\Misc\Bv\hkpBvShape.h>
#include <Physics\Collide\Shape\Misc\PhantomCallback\hkpPhantomCallbackShape.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
#include <Physics/Collide/Shape/hkpLegacyShapeType.h>
// System
#include "Systems/Common/System.hpp"
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"
#include "Systems/PhysicsHAVOK/Extras/PhantomShape.hpp"

#define PHAVOKSCENE         reinterpret_cast<HavokPhysicsScene*>(m_pSystemScene)
#define _USE_MOPP_FOR_MESH_

//
// global variables
//
extern ManagerInterfaces    g_Managers;

pcstr HavokPhysicsObject::sm_kapszTypeNames[] =
{
    "Box", "Sphere", "ConvexHull", "Mesh", "Space", "Dynamic",
    NULL
};


pcstr HavokPhysicsObject::sm_kapszCommonPropertyNames[] =
{
    "Mass", "Static", "Material", "LinearVelocity", "Quality"
};

const Properties::Property HavokPhysicsObject::sm_kaCommonDefaultProperties[] =
{
    Properties::Property( sm_kapszCommonPropertyNames[ Property_Mass ],
                          Properties::Values::Float32,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0.0f ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_Static ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          0 ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_Material ],
                          Properties::Values::String,
                          Properties::Flags::Valid,
                          "Value", NULL, NULL, NULL,
                          "" ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_LinearVelocity ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          "X", "Y", "Z", NULL,
                          0.0f, 0.0f, 0.0f ),

    Properties::Property( sm_kapszCommonPropertyNames[ Property_Quality ],
                          Properties::Values::Int32,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1 ),
};

pcstr HavokPhysicsObject::sm_kapszBoxPropertyNames[] =
{
    "Lengths",
};

const Properties::Property HavokPhysicsObject::sm_kaBoxDefaultProperties[] =
{
    Properties::Property( sm_kapszBoxPropertyNames[ BoxProperty_Lengths ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid | Properties::Flags::InitOnly |
                            Properties::Flags::WriteOnly,
                          NULL, NULL, NULL, NULL,
                          Base::Vector3::Zero ),
};

pcstr HavokPhysicsObject::sm_kapszSpherePropertyNames[] =
{
    "Radii",
};

const Properties::Property HavokPhysicsObject::sm_kaSphereDefaultProperties[] =
{
    Properties::Property( sm_kapszSpherePropertyNames[ SphereProperty_Radii ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid | Properties::Flags::InitOnly |
                            Properties::Flags::WriteOnly,
                          NULL, NULL, NULL, NULL,
                          Base::Vector3::Zero ),
};


///////////////////////////////////////////////////////////////////////////////
// HavokCharacterObject - Default constructor
HavokPhysicsObject::HavokPhysicsObject(
    ISystemScene* pSystemScene,
    pcstr pszType,
    pcstr pszName,
    hkpRigidBody* pBody
    )
    : HavokObject( pSystemScene, pszName )
    , m_Offset( Base::Vector3::Zero )
    , m_pBody( NULL )
    , m_bStatic( False )
    , m_MaterialId( -1 )
    , m_Mass( 0.0f )
    , m_LinearVelocity( Base::Vector3::Zero )
    , m_Quality( 1 )
    , m_pShapeData1( NULL )
    , m_pShapeData2( NULL )
{
    ASSERT( Property_Count == sizeof sm_kapszCommonPropertyNames /
                                sizeof sm_kapszCommonPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kapszCommonPropertyNames /
                                sizeof sm_kapszCommonPropertyNames[ 0 ] );
    ASSERT( BoxProperty_Count == sizeof sm_kapszBoxPropertyNames /
                                   sizeof sm_kapszBoxPropertyNames[ 0 ] );
    ASSERT( BoxProperty_Count == sizeof sm_kapszBoxPropertyNames /
                                   sizeof sm_kapszBoxPropertyNames[ 0 ] );
    ASSERT( SphereProperty_Count == sizeof sm_kapszSpherePropertyNames /
                                      sizeof sm_kapszSpherePropertyNames[ 0 ] );
    ASSERT( SphereProperty_Count == sizeof sm_kapszSpherePropertyNames /
                                      sizeof sm_kapszSpherePropertyNames[ 0 ] );

    //
    // Determine the object type.
    //
    if ( strcmp( pszType, sm_kapszTypeNames[ Type_Box ] ) == 0 )
    {
        m_Type = Type_Box;
    }
    else if ( strcmp( pszType, sm_kapszTypeNames[ Type_Sphere ] ) == 0 )
    {
        m_Type = Type_Sphere;
    }
    else if ( strcmp( pszType, sm_kapszTypeNames[ Type_ConvexHull ] ) == 0 )
    {
        m_Type = Type_ConvexHull;
    }
    else if ( strcmp( pszType, sm_kapszTypeNames[ Type_Mesh ] ) == 0 )
    {
        m_Type = Type_Mesh;
        m_bStatic = True;
    }
    else if ( strcmp( pszType, sm_kapszTypeNames[ Type_Space ] ) == 0 )
    {
        m_Type = Type_Space;
    }
    else if ( strcmp( pszType, sm_kapszTypeNames[ Type_Dynamic ] ) == 0 )
    {
        ASSERT( pBody != NULL );
        m_Type = Type_Dynamic;
    }
    else
    {
        ASSERT( False );
    }

    //
    // Setup pBody
    //
    if ( pBody != NULL )
    {
        //
        // Store the body information.
        //
        m_pBody = pBody;
        m_bStatic = (m_pBody->getMotionType() == hkpMotion::MotionType::MOTION_FIXED);
        m_Mass = m_pBody->getMass();

        //
        // The user data is this class.
        //
        m_pBody->setUserData( reinterpret_cast<hkUlong>(this) );

        //
        // Get the offset for the shape, if there is one.
        //
        hkVector4 Offset;
        Offset.setZero4();

        const hkpShape* pShape = pBody->getCollidable()->getShape();

        switch ( pShape->getType() )
        {
        case HK_SHAPE_CONVEX:
        case HK_SHAPE_CONVEX_TRANSFORM:
        case HK_SHAPE_CONVEX_TRANSLATE:
        case HK_SHAPE_CONVEX_PIECE:
        case HK_SHAPE_CONVEX_LIST:
        case HK_SHAPE_CONVEX_VERTICES:
            ((hkpConvexShape*)pShape)->getCentre( Offset );
            break;
        }
        Offset.store3( m_Offset );

        //
        // Synchronize the object with the body.
        //
        Update();

        //
        // Add the body reference and add it to the world.
        //
        //m_pBody->addReference();

        hkpWorld* pWorld = PHAVOKSCENE->GetWorld();
        pWorld->markForWrite();
        pWorld->addEntity( m_pBody );
        pWorld->unmarkForWrite();
    }
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokCharacterObject - Default destructor
HavokPhysicsObject::~HavokPhysicsObject(
    void
    )
{
    if ( m_pBody != NULL )
    {
        hkpWorld* pWorld = PHAVOKSCENE->GetWorld();

        pWorld->markForWrite();
        pWorld->removeEntity( m_pBody );
        pWorld->unmarkForWrite();

        m_pBody->removeReference();
    }

    //
    // Free any allocated resources.
    //
    switch ( m_Type )
    {
    //case Type_Box:
    //case Type_Sphere:
    //case Type_ConvexHull:
    //    break;

    case Type_Mesh:
        SAFE_DELETE_ARRAY( m_pShapeData1 );
        SAFE_DELETE_ARRAY( m_pShapeData2 );
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this object with the given properties
Error
HavokPhysicsObject::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Read in the properties.
    //
    Base::Vector3   Size = Base::Vector3::Zero;

    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszCommonPropertyNames[ Property_Static ] )
            {
                m_bStatic = it->GetBool( 0 );
                ASSERTMSG( (m_Type != Type_Mesh) || ((m_Type == Type_Mesh) && m_bStatic),
                           "Mesh objects can only be static objects." );

                it->ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszBoxPropertyNames[ BoxProperty_Lengths ] )
            {
                ASSERT( m_Type == Type_Box );

                Size = it->GetVector3();

                it->ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszSpherePropertyNames[ SphereProperty_Radii ] )
            {
                ASSERT( m_Type == Type_Sphere );

                Size = it->GetVector3();

                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    //
    // Create the collision geometry.
    //
    Bool                bRigidBodyInfoSet = False;
    hkpRigidBodyCinfo   RigidBodyCInfo;

    switch ( m_Type )
    {
    case Type_Box:
        if ( Size != Base::Vector3::Zero )
        {
            hkVector4 hkHalfExtent( Size.x * 0.5f, Size.y * 0.5f, Size.z * 0.5f );
            hkpBoxShape* pBox = new hkpBoxShape( hkHalfExtent, 0 );
            RigidBodyCInfo.m_shape = pBox;
            RigidBodyCInfo.m_restitution = 0.0f;  // Set restitution to 0 (no bounciness)
            bRigidBodyInfoSet = True;
        }
        break;

    case Type_Sphere:
    {
        ASSERT( Size.x != 0.0f && Size.y != 0.0f && Size.z != 0.0f );
        hkpSphereShape* pSphere = new hkpSphereShape( Size.x );
        RigidBodyCInfo.m_shape = pSphere;
        RigidBodyCInfo.m_restitution = 0.5f;  // Set restitution to 0 (no bounciness)
        bRigidBodyInfoSet = True;
        break;
    }

    case Type_ConvexHull:
    case Type_Mesh:
    case Type_Space:
        // Not handled here since we don't have the vertices yet.
        break;

    default:
        ASSERT( False );
        break;
    }

    if ( bRigidBodyInfoSet )
    {
        //
        // Convert default value to centimeters
        //
        RigidBodyCInfo.m_maxLinearVelocity *= ( 1.0f / g_Managers.pEnvironment->Variables().GetAsFloat( "Units", 1.0f ) );  

        //
        // Create the body based on the constructor info.
        //
        CreateBody( RigidBodyCInfo );
    }

    //
    // Set this as initialized.
    //
    m_bInitialized = True;

    //
    // Set the properties for this object.
    //
    SetProperties( Properties );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Get the properties for this Object
void
HavokPhysicsObject::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add the common properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaCommonDefaultProperties[ i ] );
    }

    //
    // Modify the default values.
    //
    if ( m_pBody != NULL )
    {
        Properties[ iProperty+Property_Static ].SetValue( 0, m_Mass );
        Properties[ iProperty+Property_LinearVelocity ].SetValue( m_LinearVelocity );
        Properties[ iProperty+Property_Quality ].SetValue( 0, m_Quality );
    }

    //
    // Adjust the index for the non-common properties.
    //
    iProperty += Property_Count;

    //
    // Add the box properties.
    //
    if ( m_Type == Type_Box )
    {
        Properties.reserve( Properties.size() + BoxProperty_Count );

        for ( i32 i=0; i < BoxProperty_Count; i++ )
        {
            Properties.push_back( sm_kaBoxDefaultProperties[ i ] );
        }
    }
    //
    // Add the sphere properties.
    //
    else if ( m_Type == Type_Sphere )
    {
        Properties.reserve( Properties.size() + SphereProperty_Count );

        for ( i32 i=0; i < SphereProperty_Count; i++ )
        {
            Properties.push_back( sm_kaSphereDefaultProperties[ i ] );
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Properties - Set the properties for this Object
void
HavokPhysicsObject::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    PHAVOKSCENE->GetWorld()->markForWrite();

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszCommonPropertyNames[ Property_Mass ] )
            {
                m_Mass = it->GetFloat32( 0 );

                if ( m_pBody != NULL )
                {
                    m_pBody->setMass( m_Mass );
                }
            }
            else if ( sName == sm_kapszCommonPropertyNames[ Property_Material ] )
            {
                //m_MaterialId = PHAVOKSCENE->GetMaterialId( it->GetString( 0 ) );
                //ASSERTMSG1( m_MaterialId != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 0 ) );

                if ( m_pBody != NULL )
                {
                    //HavokBodySetMaterialGroupID( m_pBody, m_MaterialId );
                }
            }
            else if ( sName == sm_kapszCommonPropertyNames[ Property_LinearVelocity ] )
            {
                ASSERT( !m_bStatic );

                m_LinearVelocity = it->GetVector3();

                if ( m_pBody != NULL )
                {
                    const hkVector4 hkVelocity( m_LinearVelocity.x, m_LinearVelocity.y,
                                                m_LinearVelocity.z );
                    m_pBody->setLinearVelocity( hkVelocity );
                }
            }
            else if ( sName == sm_kapszCommonPropertyNames[ Property_Quality ] )
            {
                m_Quality = it->GetInt32( 0 );

                if ( m_pBody != NULL )
                {
                    hkpCollidableQualityType hkQuality;

                    switch( m_Quality )
                    {
                    case 0:  hkQuality = HK_COLLIDABLE_QUALITY_DEBRIS; break;
                    case 1:  hkQuality = HK_COLLIDABLE_QUALITY_MOVING; break;
                    case 2:  hkQuality = HK_COLLIDABLE_QUALITY_BULLET; break;
                    default: hkQuality = HK_COLLIDABLE_QUALITY_CRITICAL; break;
                    }

                    m_pBody->setQualityType( hkQuality );
                }
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

    PHAVOKSCENE->GetWorld()->unmarkForWrite();
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Get system changes this Object is interested in
System::Types::BitMask
HavokPhysicsObject::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Geometry::All |
           System::Changes::Graphics::AllMesh |
           System::Changes::Physics::Velocity;
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Give this Object a change to process this system change
Error
HavokPhysicsObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    PHAVOKSCENE->GetWorld()->markForWrite();

    if ( ChangeType & System::Changes::Geometry::Position )
    {
        m_Position = *dynamic_cast<IGeometryObject*>(pSubject)->GetPosition();

        if ( m_pBody != NULL )
        {
            //
            // Modify the body's position.
            //
            Base::Vector3 TempPos = m_Position - m_Offset;
            hkVector4 Position( TempPos.x, TempPos.y, TempPos.z );
            m_pBody->setPosition( Position );
        }
    }

    if ( ChangeType & System::Changes::Geometry::Orientation )
    {
        m_Orientation = *dynamic_cast<IGeometryObject*>(pSubject)->GetOrientation();

        if ( m_pBody != NULL )
        {
            //
            // Modify the body's orientation.
            //
            hkQuaternion Orientation( m_Orientation.x, m_Orientation.y, m_Orientation.z,
                                      m_Orientation.w );
            m_pBody->setRotation( Orientation );
        }
    }

    if ( ChangeType & System::Changes::Geometry::Scale )
    {
        m_Scale = *dynamic_cast<IGeometryObject*>(pSubject)->GetScale();
    }

    if ( ChangeType & System::Changes::Physics::Velocity )
    {
        IMoveObject* pMovable = dynamic_cast<IMoveObject*>(pSubject);
        if( pMovable )
        {
            m_LinearVelocity = *pMovable->GetVelocity();
            if( m_pBody != NULL )
            {
                const hkVector4 hkVelocity( m_LinearVelocity.x, m_LinearVelocity.y, m_LinearVelocity.z );
                m_pBody->setLinearVelocity( hkVelocity );
            }
        }
    }

    if ( m_pBody == NULL )
    {
        ASSERT( m_Type == Type_Box || m_Type == Type_ConvexHull || m_Type == Type_Mesh ||
                m_Type == Type_Space );

        //
        // Any match will do, hopefully...
        //
        if ( ChangeType & System::Changes::Graphics::AllMesh )
        {
            ASSERT( (ChangeType & System::Changes::Graphics::AllMesh) ==
                    System::Changes::Graphics::AllMesh );

            //
            // Get the mesh information since we'll need it to create the convex hull.
            //
            IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);

            u32 VertexDeclCount = pGfxObj->GetVertexDeclarationCount( 0 );
            VertexDecl::Element* pVertexDecl = new VertexDecl::Element[ VertexDeclCount ];
            pGfxObj->GetVertexDeclaration( pVertexDecl, 0 );
            u32 StreamIndex = VertexDecl::FindUsageInStream( pVertexDecl, VertexDeclCount, VertexDecl::Usage::Position );
            u32 VertexSize = VertexDecl::CalculateSize( pVertexDecl, VertexDeclCount, StreamIndex );
            u32 VertexCount = pGfxObj->GetVertexCount();

            u8* pVertexBuffer = new u8[ VertexCount * VertexSize * VertexDeclCount ];
            ASSERT( pVertexBuffer != NULL );
            pGfxObj->GetVertices( pVertexBuffer, 0, StreamIndex );

            u8* pIndexBuffer = NULL;

            //
            // Get the location of the position vertex.
            //
            u32 Type;
            i32 iPosition = VertexDecl::CalculateUsageOffsetInVertex(
                                pVertexDecl, VertexDeclCount, VertexDecl::Usage::Position, Type, StreamIndex
                                );
            ASSERT( iPosition >= 0 );
            ASSERT( Type == VertexDecl::Type::Float3 || Type == VertexDecl::Type::Float4 );

            delete [] pVertexDecl;

            //
            // Adjust the points according to the scale.
            //
            if( m_Type != Type_Box )  // If not a box, calculate scale
            {
                if ( m_Scale != Base::Vector3::One )
                {
                    for ( u32 iVertex=0; iVertex < VertexCount; iVertex++ )
                    {
                        //
                        // Get the vertex at the index.
                        //
                        Base::Vector3& Vertex =
                            *reinterpret_cast<Base::Vector3*>(pVertexBuffer + iPosition + (iVertex * VertexSize));

                        Vertex.x *= m_Scale.x;
                        Vertex.y *= m_Scale.y;
                        Vertex.z *= m_Scale.z;
                    }
                }
            }

            //
            // Create the collision
            //
            hkpShape* pShape = NULL;

            if ( m_Type == Type_Box )
            {
                Base::Vector3   Min(+1.0e+38f, +1.0e+38f, +1.0e+38f);
                Base::Vector3   Max(-1.0e+38f, -1.0e+38f, -1.0e+38f);
                Base::Vector3   Adj(+1.0e+38f, +1.0e+38f, -1.0e+38f);

                for ( u32 iVertex=0; iVertex < VertexCount; iVertex++ )
                {
                    //
                    // Get the vertex at the index.
                    //
                    const Base::Vector3& Vertex =
                        *reinterpret_cast<const Base::Vector3*>(pVertexBuffer + iPosition +
                                                                (iVertex * VertexSize));

                    //
                    // Get the min and max vertices.
                    //
                    if ( Vertex.x < Min.x )
                    {
                        Min = Vertex;
                    }
                    else if ( Vertex.x == Min.x )
                    {
                        if ( Vertex.y < Min.y )
                        {
                            Min = Vertex;
                        }
                        else if ( Vertex.y == Min.y )
                        {
                            if ( Vertex.z < Min.z )
                            {
                                Min = Vertex;
                            }
                        }
                    }

                    if ( Vertex.x > Max.x )
                    {
                        Max = Vertex;
                    }
                    else if ( Vertex.x == Max.x )
                    {
                        if ( Vertex.y > Max.y )
                        {
                            Max = Vertex;
                        }
                        else if ( Vertex.y == Max.y )
                        {
                            if ( Vertex.z > Max.z )
                            {
                                Max = Vertex;
                            }
                        }
                    }

                    //
                    // Get the adjoining xy vertex to the min vertex.
                    //
                    if ( Vertex.x < Adj.x )
                    {
                        Adj = Vertex;
                    }
                    else if ( Vertex.x == Adj.x )
                    {
                        if ( Vertex.z > Adj.z )
                        {
                            Adj = Vertex;
                        }
                        else if ( Vertex.z == Adj.z )
                        {
                            if ( Vertex.y < Adj.y )
                            {
                                Adj = Vertex;
                            }
                        }
                    }
                }

                //
                // Calculate the position of the box.
                //
                Base::Vector3 HalfDiff = (Max - Min) * 0.5f;
                m_Position += Min + HalfDiff;

                //
                // Calculate the rotation of the box.
                //
                Adj -= Min;
                Adj.Normalize();

                f32 Angle = Base::Angle::ACos( Adj.Dot( Base::Vector3::UnitZ ) );

                // Todo: Change this so the orientation is added to the existing orientation, just like w/ position.
                m_Orientation.Set( Adj, Angle );

                //
                // Create the box collision.
                //
                hkVector4 HalfExtents( HalfDiff.x, HalfDiff.y, HalfDiff.z );
                pShape = new hkpBoxShape( HalfExtents );
                ASSERT( pShape != NULL );
            }
            else if ( m_Type == Type_ConvexHull )
            {
                //
                // Create the convex hull collision.
                //
                hkStridedVertices StridedVerts;
                StridedVerts.m_numVertices = VertexCount;
                StridedVerts.m_striding = VertexSize;
                StridedVerts.m_vertices = reinterpret_cast<const f32*>(pVertexBuffer + iPosition);

                hkArray<hkVector4> PlaneEquations;

                hkpConvexVerticesShape* pConvex =
                    new hkpConvexVerticesShape( StridedVerts, PlaneEquations );
                ASSERT( pConvex != NULL );

                pShape = pConvex;
            }
            else if ( m_Type == Type_Mesh )
            {
                //
                // Get the index buffer.
                //
                u32 IndexDecl = pGfxObj->GetIndexDeclaration();
                u32 IndexSize = IndexDecl::CalculateSize( IndexDecl );
                u32 IndexCount = pGfxObj->GetIndexCount();

                pIndexBuffer = new u8[ IndexCount * IndexSize ];
                ASSERT( pIndexBuffer != NULL );
                pGfxObj->GetIndices( pIndexBuffer );

                //
                // Create the mesh.
                //
                hkpMeshShape::Subpart Subpart;

                Subpart.m_vertexBase = reinterpret_cast<const f32*>(pVertexBuffer + iPosition);
                Subpart.m_vertexStriding = VertexSize;
                Subpart.m_numVertices = VertexCount;

                Subpart.m_indexBase = pIndexBuffer;
                Subpart.m_indexStriding = IndexSize;
                Subpart.m_stridingType = (IndexDecl == IndexDecl::Type::Index16) ?
                                            hkpMeshShape::INDICES_INT16 :
                                            hkpMeshShape::INDICES_INT32;
                Subpart.m_numTriangles = IndexCount / 3;

                hkpMeshShape* pMesh = new hkpMeshShape();
                ASSERT( pMesh != NULL );

                pMesh->addSubpart( Subpart );

#ifndef _USE_MOPP_FOR_MESH_
                pShape = pMesh;
#else
                //
                // Create a MOPP for the mesh which calculates faster.
                //
                hkpMoppCompilerInput mci;
                hkpMoppCode* pMoppCode = hkpMoppUtility::buildCode( pMesh, mci );

                hkpMoppBvTreeShape* pMopp = new hkpMoppBvTreeShape( pMesh, pMoppCode );
                pMoppCode->removeReference();
                pMesh->removeReference();

                pShape = pMopp;
#endif

                //
                // Keep a copy so we can delete them later.
                //
                m_pShapeData1 = pIndexBuffer;
                m_pShapeData2 = pVertexBuffer;
            }
            else if ( m_Type == Type_Space )
            {
                //
                // Create the convex hull collision.
                //
                hkStridedVertices StridedVerts;
                StridedVerts.m_numVertices = VertexCount;
                StridedVerts.m_striding = VertexSize;
                StridedVerts.m_vertices = reinterpret_cast<const f32*>(pVertexBuffer + iPosition);

                hkArray<hkVector4> PlaneEquations;

                hkpConvexVerticesShape* pConvex =
                    new hkpConvexVerticesShape( StridedVerts, PlaneEquations );
                ASSERT( pConvex != NULL );

                //
                // Create the phantom shape.
                //
                PhantomShape* pPhantom = new PhantomShape( PHAVOKSCENE );

                //
                // Create the bounding volume shape.
                //
                hkpBvShape* pBoundingVolume = new hkpBvShape( pConvex, pPhantom );
                ASSERT( pBoundingVolume != NULL );

                pShape = pBoundingVolume;
            }
            ASSERT( pShape != NULL );

            //
            // Create the body based on the collision.
            //
            hkpRigidBodyCinfo   RigidBodyCInfo;
            RigidBodyCInfo.m_shape = pShape;
            RigidBodyCInfo.m_mass = m_Mass;
            RigidBodyCInfo.m_position = hkVector4( m_Position.x, m_Position.y, m_Position.z );
            RigidBodyCInfo.m_rotation = hkQuaternion( m_Orientation.x, m_Orientation.y,
                                                      m_Orientation.z, m_Orientation.w );
            RigidBodyCInfo.m_maxLinearVelocity *= ( 1.0f / g_Managers.pEnvironment->Variables().GetAsFloat( "Units", 1.0f ) );

            CreateBody( RigidBodyCInfo );

            //
            // Free up the resources.
            //
            if ( m_Type != Type_Mesh )
            {
                delete [] pVertexBuffer;
            }
        }
    }

    PHAVOKSCENE->GetWorld()->unmarkForWrite();

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Get all system change possible for this Object
System::Changes::BitMask
HavokPhysicsObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Geometry::Position |
           System::Changes::Geometry::Orientation |
           System::Changes::POI::Contact |
           System::Changes::POI::Intersection;
}


///////////////////////////////////////////////////////////////////////////////
// CreateBody - Creates a new rigid body
void
HavokPhysicsObject::CreateBody(
    hkpRigidBodyCinfo& RigidBodyCInfo
    )
{
    //
    // Get the world.
    //
    hkpWorld* pWorld = PHAVOKSCENE->GetWorld();
    ASSERT( pWorld != NULL );

    //
    // Set static.
    //
    if ( m_bStatic )
    {
        RigidBodyCInfo.m_motionType = hkpMotion::MOTION_FIXED;
        RigidBodyCInfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
        RigidBodyCInfo.m_mass = 0.0f;
    }
    else
    {
        // Determine quality level
        hkpCollidableQualityType hkQuality;

        switch( m_Quality )
        {
        case 0:  hkQuality = HK_COLLIDABLE_QUALITY_DEBRIS; break;
        case 1:  hkQuality = HK_COLLIDABLE_QUALITY_MOVING; break;
        case 2:  hkQuality = HK_COLLIDABLE_QUALITY_BULLET; break;
        default: hkQuality = HK_COLLIDABLE_QUALITY_CRITICAL; break;
        }

        // Set values
        RigidBodyCInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;
        RigidBodyCInfo.m_qualityType = hkQuality;
        RigidBodyCInfo.m_linearVelocity = hkVector4( m_LinearVelocity.x, m_LinearVelocity.y,
                                                     m_LinearVelocity.z );
    }

    //
    // Create the body.
    //
    m_pBody = new hkpRigidBody( RigidBodyCInfo );
    ASSERT( m_pBody != NULL );

    RigidBodyCInfo.m_shape->removeReference();

    //
    // Set this object as a listener to the body.
    //
    if ( !m_bStatic )
    {
    }

    //
    // Set the material for this body.
    //
    //if ( m_MaterialId != -1 )
    //{
    //}

    //
    // The user data is this class.
    //
    m_pBody->setUserData( reinterpret_cast<hkUlong>(this) );

    //
    // Add the body to the world.
    //
    pWorld->markForWrite();
    pWorld->addEntity( m_pBody );
    pWorld->unmarkForWrite();
}


///////////////////////////////////////////////////////////////////////////////
// Update - Update this Object (should be called every frame)
void
HavokPhysicsObject::Update(
    f32 DeltaTime
    )
{
    UNREFERENCED_PARAM( DeltaTime );

    const hkVector4& Position = m_pBody->getCenterOfMassInWorld();
    const hkQuaternion& Orientation = m_pBody->getRotation();
    const hkVector4& LinearVelocity = m_pBody->getLinearVelocity();

    Position.store3( m_Position );

    Orientation.getImag().store3( &m_Orientation.x );
    m_Orientation.w = Orientation.getReal();

    LinearVelocity.store3( (hkReal*)&m_LinearVelocity );

    PostChanges( System::Changes::Geometry::Position | System::Changes::Geometry::Orientation );
}


///////////////////////////////////////////////////////////////////////////////
// AddContact - Adds a new contact 
// (this info might be interesting to other systems)
void 
HavokPhysicsObject::AddContact( 
    const IContactObject::Info& ContactInfo 
    )
{
    m_ContactInfo = ContactInfo;

    PostChanges( System::Changes::POI::Contact );
}


///////////////////////////////////////////////////////////////////////////////
// AddIntersection - Adds a new intersection 
// (this info might be interesting to other systems)
void
HavokPhysicsObject::AddIntersection(
    const IIntersectionObject::Info& IntersectionInfo
    )
{
    m_aIntersectionInfo.push_back( IntersectionInfo );

    PostChanges( System::Changes::POI::Intersection );
}


///////////////////////////////////////////////////////////////////////////////
// GetContact - Get the list of contacts
const IContactObject::Info* 
HavokPhysicsObject::GetContact( 
    void 
    )
{
    return &m_ContactInfo;
}


///////////////////////////////////////////////////////////////////////////////
// IsStatic - Return true if this is a static object (doesn't move)
const Bool 
HavokPhysicsObject::IsStatic( 
    void 
    )
{
    return m_bStatic;
}


///////////////////////////////////////////////////////////////////////////////
// GetIntersections - Get the list of intersections
const IIntersectionObject::InfoArray&
HavokPhysicsObject::GetIntersections(
    void
    )
{
    return m_aIntersectionInfo;
}
