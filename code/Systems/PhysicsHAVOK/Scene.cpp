// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// External
#define HK_COMPAT_FILE  <Common\Compat\hkCompatVersions.h>
#define HK_CLASSES_FILE <Common\Serialize\ClassList\hkPhysicsClasses.h>
#include <Common\Base\hkBase.h>
#include <Common\Compat\hkCompat_All.cxx>
#include <Common\SceneData\Graph\hkxNode.h>
#include <Common\SceneData\Scene\hkxScene.h>
#include <Common\Serialize\Util\hkLoader.h>
#include <Common\Serialize\Util\hkRootLevelContainer.h>
#include <Common\Serialize\Util\hkBuiltinTypeRegistry.cxx>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Physics\Dynamics\Entity\hkpEntityListener.h>
#include <Physics\Dynamics\Entity\hkpEntityActivationListener.h>
#include <Physics\Dynamics\Phantom\hkpSimpleShapePhantom.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Collide\Dispatch\hkpAgentRegisterUtil.h>
#include <Physics\Collide\Filter\Group\hkpGroupFilterSetup.h>
#include <Physics\Collide\Shape\Compound\Collection\ExtendedMeshShape\hkpExtendedMeshShape.h>
#include <Physics\Collide\Shape\Compound\Collection\List\hkpListShape.h>
#include <Physics\Collide\Shape\Compound\Collection\SimpleMesh\hkpSimpleMeshShape.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppBvTreeShape.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppCompilerInput.h>
#include <Physics\Collide\Shape\Compound\Tree\Mopp\hkpMoppUtility.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>
#include <Physics\Utilities\CharacterControl\CharacterProxy\hkpCharacterProxy.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
#include <Physics\Utilities\Dynamics\Inertia\hkpInertiaTensorComputer.h>
#include <Physics\Utilities\Serialize\hkpPhysicsData.h>
#include <Physics\Dynamics\Collide\hkpCollisionListener.h>
#ifdef __HAVOK_VDB__
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#endif
// System
#include "Systems/PhysicsHAVOK/ServiceCollision.hpp"
#include "Systems/PhysicsHAVOK/System.hpp"
#include "Systems/PhysicsHAVOK/Scene.hpp"
#include "Systems/PhysicsHAVOK/Task.hpp"
#include "Systems/PhysicsHAVOK/Object.hpp"
#include "Systems/PhysicsHAVOK/ObjectPhysics.hpp"
#include "Systems/PhysicsHAVOK/ObjectCharacter.hpp"

///////////////////////////////////////////////////////////////////////////////
// CollisionListener - Local helper classes
class CollisionListener : public hkpCollisionListener
{
    virtual void  contactPointAddedCallback (hkpContactPointAddedEvent &event){}
    virtual void  contactPointConfirmedCallback (hkpContactPointConfirmedEvent &event);
    virtual void  contactPointRemovedCallback (hkpContactPointRemovedEvent &event){}
};


///////////////////////////////////////////////////////////////////////////////
// contactPointConfirmedCallback - Callback that handles when an object collide
void CollisionListener::contactPointConfirmedCallback(hkpContactPointConfirmedEvent &event)
{
    f32 Impact = -event.m_projectedVelocity;

    static const f32 skfMaxImpact = 100.0f;

    // Filter out collisions with low impact 
    // (prevent collision messages spamming)
    if( Impact > skfMaxImpact )
    {
        // Get physics object for bodyA
        hkpEntity* pEntityA = (hkpEntity*)event.m_collidableA->getRootCollidable()->getOwner();
        HavokPhysicsObject* pObjectA = (HavokPhysicsObject*)pEntityA->getUserData();

        // Get physics object for bodyB
        hkpEntity* pEntityB = (hkpEntity*)event.m_collidableB->getRootCollidable()->getOwner();
        HavokPhysicsObject* pObjectB = (HavokPhysicsObject*)pEntityB->getUserData();

        // Fill out physics object
        IContactObject::Info ContactInfo;

        const hkVector4& Position = event.m_contactPoint->getPosition();
        Position.store3( ContactInfo.m_Position );

        const hkVector4& Normal = event.m_contactPoint->getNormal();
        Normal.store3( ContactInfo.m_Normal );

        ContactInfo.m_Impact = Impact;
        ContactInfo.m_Static = ( pEntityA->getMotion()->m_type == hkpMotion::MOTION_FIXED || pEntityB->getMotion()->m_type == hkpMotion::MOTION_FIXED );

        ContactInfo.m_VelocityObjectA = Base::Vector3::Zero;
        ContactInfo.m_VelocityObjectB = Base::Vector3::Zero;

        // Tell the objects about the contact
        if( pObjectA )
        {
            const hkVector4& linA = pObjectA->GetRigidBody()->getLinearVelocity();
            linA.store3( ContactInfo.m_VelocityObjectA );
            pObjectA->AddContact( ContactInfo );
        }

        if( pObjectB )
        {
            const hkVector4& linB = pObjectB->GetRigidBody()->getLinearVelocity();
            linB.store3( ContactInfo.m_VelocityObjectB );
            pObjectB->AddContact( ContactInfo );
        }
    }
}


//
// global variables
//
extern ManagerInterfaces    g_Managers;


const Base::Vector3 HavokPhysicsScene::sm_kDefaultGravity(0.0f, -9.8f, 0.0f);


pcstr HavokPhysicsScene::sm_kapszPropertyNames[] =
{
    "SceneFile", "Gravity", "Material", "Elasticity", "Friction", "Softness",
};

const Properties::Property HavokPhysicsScene::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_SceneFile ],
                          VALUE4( Properties::Values::Path, Properties::Values::String,
                          Properties::Values::Float32, Properties::Values::Float32 ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Path", "Type", "Value1", "Value2",
                          "", "", 0.0f, 0.0f ),

    Properties::Property( sm_kapszPropertyNames[ Property_Gravity ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          sm_kDefaultGravity ),

    Properties::Property( sm_kapszPropertyNames[ Property_Material ],
                          Properties::Values::String,
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          "Value", NULL, NULL, NULL,
                          "" ),

    Properties::Property( sm_kapszPropertyNames[ Property_Elasticity ],
                          VALUE3( Properties::Values::String, Properties::Values::String,
                                  Properties::Values::Float32 ),
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          "Material1", "Material2", "Coefficient", NULL,
                          "", "", 0.0f ),

    Properties::Property( sm_kapszPropertyNames[ Property_Friction ],
                          VALUE4( Properties::Values::String, Properties::Values::String,
                                  Properties::Values::Float32, Properties::Values::Float32 ),
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          "Material1", "Material2", "Static", "Kinetic",
                          "", "", 0.0f, 0.0f ),

    Properties::Property( sm_kapszPropertyNames[ Property_Softness ],
                          VALUE3( Properties::Values::String, Properties::Values::String,
                                  Properties::Values::Float32 ),
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          "Material1", "Material2", "Value", NULL,
                          "", "", 0.0f ),
};



///////////////////////////////////////////////////////////////////////////////
// HavokPhysicsScene - Default constructor
HavokPhysicsScene::HavokPhysicsScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )
    , m_pTask( NULL )
    , m_pWorld( NULL )
    //, m_Gravity( sm_kDefaultGravity )
    , m_pLoader( NULL )
    , m_kVelocityModifier( 0.5f, 0.5f, 0.5f )
    , m_kfMass( 50.0f )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokPhysicsScene - Default destructor
HavokPhysicsScene::~HavokPhysicsScene(
    void
    )
{
    if ( m_pWorld != NULL )
    {
        m_pWorld->markForWrite();

        //
        // Delete the task first because it attached itself to the world as a listener.
        //
        SAFE_DELETE( m_pTask );

        //
        // Delete all the break-off-parts bodies.
        //
        for ( hkArray<hkpRigidBody*>::iterator it=m_aUnattachedBodies.begin();
              it != m_aUnattachedBodies.end(); it++ )
        {
            hkpRigidBody* pBody = *it;
            m_pWorld->removeEntity( pBody );
            pBody->removeReference();
        }
        m_aUnattachedBodies.clear();

        //
        // Delete all the break-off-parts utilities.
        //
        for ( hkArray<hkpBreakOffPartsUtil*>::iterator it=m_apBreakOffPartsUtils.begin();
              it != m_apBreakOffPartsUtils.end(); it ++ )
        {
            (*it)->removeReference();
        }
        m_apBreakOffPartsUtils.clear();


        m_pWorld->removeReference();

        //
        // Can't do this since the world is deleted at this point.  Can't do it before the reference
        //  because it needs the mark for write ro remove the reference.
        //
        //m_pWorld->unmarkForWrite();
    }

    if ( m_pLoader != NULL )
    {
        reinterpret_cast<hkLoader*>(m_pLoader)->removeReference();
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Scene
System::Type
HavokPhysicsScene::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GlobalSceneStatusChanged - Called from the framework to inform the scene 
// extension of the overall scene status.
void
HavokPhysicsScene::GlobalSceneStatusChanged(
    GlobalSceneStatus Status
    )
{
    if ( m_pWorld != NULL  &&  Status == ISystemScene::GlobalSceneStatus::PostLoadingObjects )
    {
        //
        // Create the loader.
        //
        if ( m_pLoader == NULL )
        {
            m_pLoader = new hkLoader();
            ASSERT( m_pLoader != NULL );
        }
        hkLoader* pLoader = reinterpret_cast<hkLoader*>(m_pLoader);

        //
        // Iterate through all the scene files, load them, and create the contents.
        //
        for ( SceneFiles::iterator itSF=m_SceneFiles.begin(); itSF != m_SceneFiles.end();
              itSF++ )
        {
            //
            // Load the scene file and get the physics data.
            //
            hkRootLevelContainer* pRootLevelContainer = pLoader->load( itSF->sPathName.c_str() );
            ASSERT( pRootLevelContainer != NULL );

            hkxScene* pScene =
                reinterpret_cast<hkxScene*>(pRootLevelContainer->findObjectByType(
                    hkxSceneClass.getName()
                    ));

            hkpPhysicsData* pPhysicsData =
                reinterpret_cast<hkpPhysicsData*>(pRootLevelContainer->findObjectByType(
                    hkpPhysicsDataClass.getName()
                    ));
            ASSERT( pPhysicsData != NULL );
            pPhysicsData->addReference();

            //
            // Break off parts data.
            //
            hkpBreakOffPartsUtil* pBreakOffPartsUtil = NULL;
            f32 BreakImpulse = 250.0f;

            if ( itSF->Type == SceneFileTypes::BreakOffParts )
            {
                //
                // Create a break of parts utility and save it.
                //
                m_pWorld->markForWrite();
                pBreakOffPartsUtil = new hkpBreakOffPartsUtil( m_pWorld, this );
                m_pWorld->unmarkForWrite();
                ASSERT( pBreakOffPartsUtil != NULL );

                m_apBreakOffPartsUtils.pushBack( pBreakOffPartsUtil );

                if ( itSF->Value1 != 0.0f )
                {
                    BreakImpulse = itSF->Value1;
                }
            }

            //
            // Iterate through all the physics systems.
            //
            for ( hkArray<hkpPhysicsSystem*>::const_iterator itPS=pPhysicsData->getPhysicsSystems().begin();
                  itPS != pPhysicsData->getPhysicsSystems().end(); itPS++ )
            {
                //
                // Iterate through all the rigid bodies.
                //
                const hkArray<hkpRigidBody*>& kRigidBodies = (*itPS)->getRigidBodies();

                for ( hkArray<hkpRigidBody*>::const_iterator itRB=kRigidBodies.begin();
                      itRB != kRigidBodies.end(); itRB++ )
                {
                    //
                    // Get the rigid body and its node.
                    //
                    hkpRigidBody* pBody = *itRB;
                    hkxNode* pRigidBodyNode = pScene->findNodeByName( pBody->getName() );

                    //
                    // Handle the objects in the scene differently depending on the scene type.
                    // This limits us to having all the objects in the scene be of the same type.
                    //
                    if ( itSF->Type == SceneFileTypes::Extend )
                    {
                        const hkpShape* pShape = pBody->getCollidable()->getShape();

                        ExtensionData ed;
                        ed.pszName = pBody->getName();
                        ed.pBody = pBody;
                        m_aExtensions[ (void*)pBody ] = ed;

                        //
                        // Inform the universal scene we want to extend an object.
                        //
                        PostChanges( System::Changes::Generic::ExtendObject );
                    }
                    else if ( itSF->Type == SceneFileTypes::Loose )
                    {
                        //
                        // Create a new body based on this body.
                        //
                        hkpRigidBodyCinfo RigidBodyCInfo;
                        pBody->getCinfo( RigidBodyCInfo );
                        RigidBodyCInfo.m_maxLinearVelocity *=
                            ( 1.0f / g_Managers.pEnvironment->Variables().GetAsFloat( "Units", 1.0f ) );

                        hkpRigidBody* pNewBody = new hkpRigidBody( RigidBodyCInfo );
                        ASSERT( pNewBody != NULL );

                        //
                        // Add the body to the world.
                        //
                        m_pWorld->markForWrite();
                        m_pWorld->addEntity( pNewBody );
                        m_pWorld->unmarkForWrite();

                        m_aUnattachedBodies.pushBack( pNewBody );
                    }
                    else if ( itSF->Type == SceneFileTypes::BreakOffParts )
                    {
                        //
                        // Get the name of the different shapes so that they can be associated with
                        //  a universal object later on when they break off.
                        //
                        const hkpShape* pShape = pBody->getCollidable()->getShape();
                        const hkpShapeContainer* pShapeContainer = pShape->getContainer();

                        if ( pShapeContainer != NULL )
                        {
                            hkpShapeContainer::ShapeBuffer ShapeBuffer;

                            //
                            // Convert all the child shapes into an extended mesh shape.
                            //
                            hkpExtendedMeshShape* pExtendedMeshShape = new hkpExtendedMeshShape();

                            hkArray<const hkpConvexShape*> ConvexShapes;

                            //
                            // The nodes are sturctured differently from the physics data.  The
                            //  first shape name is in the main node and then all the sub nodes have
                            //  the other names.
                            //
                            pcstr pszShapeName = pRigidBodyNode->m_name;

                            //
                            // Iterate through all the child shapes.
                            //
                            for ( hkpShapeKey ShapeKey=pShapeContainer->getFirstKey();
                                  ShapeKey < HK_INVALID_SHAPE_KEY;
                                  ShapeKey=pShapeContainer->getNextKey( ShapeKey ) )
                            {
                                //
                                // Get the child shape.
                                //
                                hkpShape* pChildShape =
                                    const_cast<hkpShape*>(
                                        pShapeContainer->getChildShape( ShapeKey, ShapeBuffer )
                                        );

                                //
                                // Set the name mapping...
                                //
                                if ( pChildShape->getType() != HK_SHAPE_TRIANGLE )
                                {
                                    pChildShape->setUserData( (hkUlong)pszShapeName );
                                }

                                //
                                // ... and get the name of the name shape.
                                //
                                if ( ShapeKey < pRigidBodyNode->m_numChildren )
                                {
                                    pszShapeName = pRigidBodyNode->m_children[ ShapeKey ]->m_name;
                                }

                                switch ( pChildShape->getType() )
                                {
                                case HK_SHAPE_CONVEX:
                                case HK_SHAPE_CONVEX_TRANSFORM:
                                case HK_SHAPE_CONVEX_TRANSLATE:
                                case HK_SHAPE_CONVEX_PIECE:
                                case HK_SHAPE_CONVEX_LIST:
                                case HK_SHAPE_CONVEX_VERTICES:
                                case HK_SHAPE_PACKED_CONVEX_VERTICES:
                                    ConvexShapes.pushBack( (hkpConvexShape*)pChildShape );
                                    break;

                                case HK_SHAPE_MOPP:
                                {
                                    hkpMoppBvTreeShape* pMoppShape =
                                        (hkpMoppBvTreeShape*)pChildShape;

                                    if ( pMoppShape->getShapeCollection()->getType() ==
                                         HK_SHAPE_TRIANGLE_COLLECTION )
                                    {
                                        hkpSimpleMeshShape* pSimpleMeshShape =
                                            (hkpSimpleMeshShape*)pMoppShape->getShapeCollection();

                                        hkpExtendedMeshShape::TrianglesSubpart TriSubpart;
                                        TriSubpart.m_numTriangleShapes = pSimpleMeshShape->m_triangles.getSize();
                                        TriSubpart.m_indexBase = pSimpleMeshShape->m_triangles.begin();
                                        TriSubpart.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;
                                        TriSubpart.m_indexStriding = sizeof (hkpSimpleMeshShape::Triangle);
                                        TriSubpart.m_numVertices = pSimpleMeshShape->m_vertices.getSize();
                                        TriSubpart.m_vertexBase = (f32*)pSimpleMeshShape->m_vertices.begin();
                                        TriSubpart.m_vertexStriding = sizeof (hkVector4);
                                        pExtendedMeshShape->addTrianglesSubpart( TriSubpart );
                                    }
                                    else
                                    {
                                        ASSERTMSG( False, "Invalid shape collection." );
                                    }
                                    break;
                                }

                                default:
                                    ASSERTMSG( False, "Invalid shape type." );
                                    break;
                                }
                            }

                            //
                            // Create the extended mesh shape.
                            //
                            hkpExtendedMeshShape::ShapesSubpart ShapesSubpart(
                                ConvexShapes.begin(), ConvexShapes.getSize(),
                                hkTransform::getIdentity()
                                );
                            ShapesSubpart.m_rotationSet = ShapesSubpart.m_offsetSet = false;

                            pExtendedMeshShape->addShapesSubpart( ShapesSubpart );

                            //
                            // Convert it into a bv tree.
                            //
                            hkpMoppCompilerInput mci;
                            mci.m_enableChunkSubdivision = true;

                            hkpMoppCode* pMoppCode =
                                hkpMoppUtility::buildCode( pExtendedMeshShape, mci );
                            hkpMoppBvTreeShape* pBvTreeShape =
                                new hkpMoppBvTreeShape( pExtendedMeshShape,  pMoppCode );

                            pMoppCode->removeReference();
                            pExtendedMeshShape->removeReference();

                            //
                            // Create a new body and add it to the world.
                            //
                            hkpRigidBodyCinfo ci;
                            ci.m_shape = pBvTreeShape;
                            ci.m_motionType = hkpMotion::MOTION_FIXED;
                            ci.m_numUserDatasInContactPointProperties = 1;
                            ci.m_maxAngularVelocity *= 100.0f;
                            ci.m_maxLinearVelocity *= 100.0f;
                            ci.m_position = pBody->getPosition();
                            ci.m_rotation = pBody->getRotation();

                            hkpRigidBody* pBreakableBody = new hkpRigidBody( ci );
                            pBvTreeShape->removeReference();

                            m_pWorld->markForWrite();
                            m_pWorld->addEntity( pBreakableBody );

                            for ( i32 i=0; i < ConvexShapes.getSize(); i++ )
                            {
                                pBreakOffPartsUtil->markPieceBreakable(
                                    pBreakableBody, 0x80000000 | i, BreakImpulse
                                    );
                            }
                            m_pWorld->unmarkForWrite();

                            m_aUnattachedBodies.pushBack( pBreakableBody );
                        }
                    }
                    else
                    {
                        ASSERTMSG( False, "Unhandled scene type." );
                    }
                }

                #ifdef ADD_CONSTRAINTS
                // NOTE: This section is yet to be implemented.

                //
                // Iterate through all the constraints.
                //
                const hkArray<hkpConstraintInstance*>& kConstraints = (*itPS)->getConstraints();

                for ( hkArray<hkpConstraintInstance*>::const_iterator itCI=kConstraints.begin();
                      itCI != kConstraints.end(); itCI++ )
                {
                    //m_pWorld->addConstraint( *itCI );
                }
                #endif
            }
        }

        //pLoader->removeReference();
    }
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Scene with the given properties
Error
HavokPhysicsScene::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Read the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_SceneFile ] )
            {
                SceneFileData sfd;
                sfd.sPathName = it->GetString( 0 );

                std::string sType =  it->GetString( 1 );

                if ( sType == "Extend" )
                {
                    sfd.Type = SceneFileTypes::Extend;
                }
                else if ( sType == "Loose" )
                {
                    sfd.Type = SceneFileTypes::Loose;
                }
                else if ( sType == "BreakOffParts" )
                {
                    sfd.Type = SceneFileTypes::BreakOffParts;
                }
                else
                {
                    ASSERTMSG( False, "Unknown scene type." );
                }

                sfd.Value1 = it->GetFloat32( 2 );
                sfd.Value2 = it->GetFloat32( 3 );

                m_SceneFiles.push_back( sfd );

                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    //
    // Create the world with default values.
    //
    hkpWorldCinfo WorldCInfo;
    WorldCInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
    WorldCInfo.m_gravity.set( 0, -9.8f, 0 );
    WorldCInfo.m_collisionTolerance = 0.1f; 
    WorldCInfo.setBroadPhaseWorldSize( 100000.0f );
    WorldCInfo.setupSolverInfo( hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM );
    WorldCInfo.m_expectedMaxLinearVelocity *= 1.0f / g_Managers.pEnvironment->Variables().GetAsFloat( "Units", 1.0f );

    // havokdaniel's suggestion
    // this was necessary for version 5.5.0 of Havok to run multithreaded
    WorldCInfo.m_processToisMultithreaded = false;

    m_pWorld = new hkpWorld( WorldCInfo );
    ASSERT( m_pWorld != NULL );

    m_pWorld->markForWrite();

    hkpAgentRegisterUtil::registerAllAgents( m_pWorld->getCollisionDispatcher() );

    //
    // Add a collision filter to the world to allow the bodies interpenetrate
    //
    {
        hkpGroupFilter* Filter = new hkpGroupFilter();
        m_pWorld->setCollisionFilter( Filter );
        Filter->removeReference();
    }

    //
    // Add collision listener
    //
    {
        CollisionListener* pListener = new CollisionListener();
        m_pWorld->addCollisionListener( pListener );
    }

    //
    // Add the default material to the list.
    //
    //Material m;
    //m.Name = "Default";
    //m.Id = HavokMaterialGetDefaultGroupID( m_pWorld );
    //m_Materials.push_back( m );

    //
    // Create the task for simulating physics.
    //
    m_pTask = new HavokPhysicsTask( this, m_pWorld );
    ASSERT( m_pTask != NULL );

    m_pWorld->unmarkForWrite();

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    //
    // Set the properties for this scene.
    //
    SetProperties( Properties );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Scene are returned in Properties
void
HavokPhysicsScene::GetProperties(
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
    if ( m_pWorld != NULL )
    {
        const hkVector4 hkGravity = m_pWorld->getGravity();

        Base::Vector3 Gravity;
        hkGravity.store3( Gravity );
        Properties[ iProperty+Property_Gravity ].SetValue( Gravity );
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Scene
void
HavokPhysicsScene::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    m_pWorld->markForWrite();

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_Gravity ] )
            {
                ASSERT( m_pWorld != NULL );

                //
                // Set the gravity.
                //
                const Base::Vector3& Gravity = it->GetVector3();

                hkVector4 hkGravity( Gravity.x, Gravity.y, Gravity.z );
                m_pWorld->setGravity( hkGravity );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Material ] )
            {
                const std::string sMaterialName = it->GetString( 0 );
                ASSERTMSG1( GetMaterialId( sMaterialName ) == -1,
                            "Physical material %s already exists.",
                            it->GetStringPtr( 0 ) );

                //
                // Create the material and add it to the list.
                //
                //Material m;
                //m.Name = sMaterialName;
                //m.Id = HavokMaterialCreateGroupID( m_pWorld );

                //m_Materials.push_back( m );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Elasticity ] )
            {
                //i32 Material1Id = GetMaterialId( it->GetString( 0 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 0 ) );

                //i32 Material2Id = GetMaterialId( it->GetString( 1 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 1 ) );

                //f32 Coefficient = it->GetFloat32( 2 );

                //
                // Set the elasticity between the two materials.
                //
                //HavokMaterialSetDefaultElasticity(
                //    m_pWorld, Material1Id, Material2Id, Coefficient
                //    );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Friction ] )
            {
                //i32 Material1Id = GetMaterialId( it->GetString( 0 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 0 ) );

                //i32 Material2Id = GetMaterialId( it->GetString( 1 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 1 ) );

                //f32 Static = it->GetFloat32( 2 );
                //f32 Kinetic = it->GetFloat32( 3 );

                //
                // Set the friction between the two materials.
                //
                //HavokMaterialSetDefaultFriction(
                //    m_pWorld, Material1Id, Material2Id, Static, Kinetic
                //    );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Softness ] )
            {
                //i32 Material1Id = GetMaterialId( it->GetString( 0 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 0 ) );

                ////i32 Material2Id = GetMaterialId( it->GetString( 1 ) );
                //ASSERTMSG1( Material1Id != -1, "Physical material %s does not exist.",
                //            it->GetStringPtr( 1 ) );

                //f32 Value = it->GetFloat32( 2 );

                //
                // Set the softness between the two materials.
                //
                //HavokMaterialSetDefaultSoftness(
                //    m_pWorld, Material1Id, Material2Id, Value
                //    );
            }
            else
            {
                ASSERTMSG( False, "Unknown property" );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }

    m_pWorld->unmarkForWrite();
}


///////////////////////////////////////////////////////////////////////////////
// GetObjectTypes - Get Object types for this Scene
pcstr*
HavokPhysicsScene::GetObjectTypes(
    void
    )
{
    return HavokPhysicsObject::sm_kapszTypeNames;
}


///////////////////////////////////////////////////////////////////////////////
// CreateObject - Create an Object with the given Name and Type
ISystemObject*
HavokPhysicsScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pszType != NULL );
    ASSERT( pszName != NULL );

    if( strcmp( pszType, "Character" ) == 0 )
    {
        HavokCharacterObject* pObject = new HavokCharacterObject( this, pszName );
        pObject->SetType( pszType );

        m_Characters.push_back( pObject );
        return pObject;
    }
    else
    {
        HavokPhysicsObject* pObject = new HavokPhysicsObject( this, pszType, pszName );
        pObject->SetType( pszType );
        return pObject;
    }
}


///////////////////////////////////////////////////////////////////////////////
// DestroyObject - Destorys the given Object, removing it from the Scene
Error
HavokPhysicsScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a HavokCharacterObject or HavokPhysicsObject so that the correct destructor will be called.
    //
    HavokObject* pObject = reinterpret_cast<HavokObject*>(pSystemObject);
    if( strcmp( pObject->GetType(), "Character" ) == 0 )
    {
        HavokCharacterObject* pCharacterObject = reinterpret_cast<HavokCharacterObject*>(pSystemObject);
        SAFE_DELETE( pCharacterObject );
    }
    else
    {
        HavokPhysicsObject* pPhysicsObject = reinterpret_cast<HavokPhysicsObject*>(pSystemObject);
        SAFE_DELETE( pPhysicsObject );
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemTask - Returns the task associated with this Scene
ISystemTask*
HavokPhysicsScene::GetSystemTask(
    void
    )
{
    return m_pTask;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Scene
System::Changes::BitMask
HavokPhysicsScene::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Generic::ExtendObject;
}


///////////////////////////////////////////////////////////////////////////////
// GetCreateObjects - Returns the creation data of all the universal objects to create.
void
HavokPhysicsScene::GetCreateObjects(
    std::vector<CreateObjectData>& apszNames
    )
{
    UNREFERENCED_PARAM( apszNames );
    ASSERT( False, "Not implemented." );
}


///////////////////////////////////////////////////////////////////////////////
// GetDestroyObjects - Returns the names of all the universal objects to destroy.
void
HavokPhysicsScene::GetDestroyObjects(
    std::vector<pcstr>& apszNames
    )
{
    UNREFERENCED_PARAM( apszNames );
    ASSERT( False, "Not implemented." );
}


///////////////////////////////////////////////////////////////////////////////
// GetExtendObjects - Returns the names of all the universal objects to extend.
void
HavokPhysicsScene::GetExtendObjects(
    ExtendObjectDataArray& apszNames
    )
{
    std::lock_guard<std::mutex> lock( m_BrokenOffPartsSpinWait );

    for ( std::map<void*, ExtensionData>::iterator it=m_aExtensions.begin();
          it != m_aExtensions.end(); it++ )
    {
        IGenericScene::ExtendObjectData eod;
        eod.pszName = it->second.pszName;
        eod.pUserData = it->second.pBody;
        apszNames.push_back( eod );
    }
    m_aExtensions.clear();
}


///////////////////////////////////////////////////////////////////////////////
// GetUnextendObjects - Returns the names of all the universal objects to unextend.
void
HavokPhysicsScene::GetUnextendObjects(
    std::vector<pcstr>& apszNames
    )
{
    UNREFERENCED_PARAM( apszNames );
    ASSERT( False, "Not implemented." );
}


///////////////////////////////////////////////////////////////////////////////
// ExtendObject - Returns the universal object extension.
ISystemObject*
HavokPhysicsScene::ExtendObject(
    pcstr pszName,
    void* pUserData
    )
{
    ASSERT( pszName != NULL );
    ASSERT( pUserData != NULL );

    //
    // Get the body and create the object.
    //
    hkpRigidBody* pBody = reinterpret_cast<hkpRigidBody*>(pUserData);
    ASSERT( pBody != NULL );

    HavokPhysicsObject* pObject = new HavokPhysicsObject( this, "Dynamic", pszName, pBody );
    ASSERT( pObject != NULL );

    //
    // Set the type and set the object as activated.
    //
    pObject->SetType( "Dynamic" );

    m_pTask->SetObjectActivation( pObject );

    return pObject;
}


///////////////////////////////////////////////////////////////////////////////
// UnextendObject - Informs the ISystemScene of the object extension being removed.
ISystemObject*
HavokPhysicsScene::UnextendObject(
    pcstr pszName
    )
{
    UNREFERENCED_PARAM( pszName );
    ASSERT( False, "Not implemented." );
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// GetMaterialId - Returns the ID for the given material name
i32
HavokPhysicsScene::GetMaterialId(
    const std::string& sName
    )
{
    i32 Id = -1;

    for ( Materials::const_iterator it=m_Materials.begin(); it != m_Materials.end(); it++ )
    {
        if ( (*it).Name == sName )
        {
            Id = (*it).Id;
            break;
        }
    }

    return Id;
}


///////////////////////////////////////////////////////////////////////////////
// breakOffSubPart - Callback when a piece needs to be broken off
hkResult
HavokPhysicsScene::breakOffSubPart(
   const ContactImpulseLimitBreachedEvent& Event,
   hkArray<hkpShapeKey>& KeysBrokenOff,
   hkpPhysicsSystem& PhysicsSystem
)
{
    hkResult Result = HK_FAILURE;

    hkInplaceArray<const hkpBreakOffPartsListener::ContactImpulseLimitBreachedEvent::PointInfo,4>::iterator Iterator = Event.m_points.begin();

    for( ; Iterator != Event.m_points.end(); Iterator++ )
    {
        //
        // Associate the broken pieces with a universal object.
        //
        const hkpBreakOffPartsListener::ContactImpulseLimitBreachedEvent::PointInfo* pPointInfo = Iterator;
        hkpShapeKey BrokenPieceKey = pPointInfo->m_brokenShapeKey;
        hkpRigidBody* pBreakingBody = Event.m_breakingBody;
        hkpRigidBody* pCollidingBody = pPointInfo->m_collidingBody;

        //
        // Determine what kind of shape it is.
        //
        hkpShape* pShape = const_cast<hkpShape*>(pBreakingBody->getCollidable()->getShape());
        const hkpShape* pBrokenOffShape = HK_NULL;

        switch ( pShape->m_type )
        {
        case HK_SHAPE_LIST:
            {
                hkpListShape* pShapeList = reinterpret_cast<hkpListShape*>( pShape );
                pBrokenOffShape = pShapeList->m_childInfo[ BrokenPieceKey ].m_shape;
                break;
            }

        case HK_SHAPE_MOPP:
            {
                hkpShapeContainer::ShapeBuffer ShapeBuffer;
                hkpMoppBvTreeShape* pMoppShape = reinterpret_cast<hkpMoppBvTreeShape*>( pShape );
                pBrokenOffShape = pMoppShape->getShapeCollection()->getChildShape(
                    BrokenPieceKey, ShapeBuffer
                    );
                ASSERT( (void*)pBrokenOffShape != (void*)ShapeBuffer );
                break;
            }

        default:
            ASSERTMSG( False, "This shape is not implemented yet" );
        }

        //
        // Make sure we have a shape that has volume.
        //
        if ( pBrokenOffShape != NULL )
        {
            hkAabb Aabb;
            pBrokenOffShape->getAabb( hkTransform::getIdentity(), 0.0f, Aabb );

            hkVector4 Diff;
            Diff.setSub4(Aabb.m_max, Aabb.m_min);
            Diff.setAbs4( Diff );

            Base::Vector3 vDiff;
            Diff.store3( vDiff );

            if ( vDiff.x < 0.1f || vDiff.y < 0.1f || vDiff.z < 0.1f )
            {
                pBrokenOffShape = NULL;
            }
        }

        //
        // Check to see if it has already been detached by another thread.
        //
        Bool bProcessDetach = False;

        if ( pBrokenOffShape != NULL )
        {

            //
            // The first thread to add this to the map wins.
            //
            std::lock_guard<std::mutex> lock( m_BrokenOffPartsSpinWait );

            //
            // Check to see if the part isn't already in the map.
            //
            if ( m_aExtensions.find( (void*)pBrokenOffShape ) == m_aExtensions.end() )
            {
                //
                // Add a dummy entry.
                //
                ExtensionData ed;
                ::memset( &ed, 0, sizeof ed );
                m_aExtensions[ (void*)pBrokenOffShape ] = ed;

                bProcessDetach = True;
            }
            else
            {
                Result = HK_SUCCESS;
            }
        }

        //
        // Detach the shape from the big body and create new a body for the broken off shape.
        //
        if ( bProcessDetach )
        {
            //
            // Create a body.
            //
            hkpRigidBody* pNewBody = NULL;

            hkVector4 LinearVelocity = pCollidingBody->getLinearVelocity();
            hkVector4 AngularVelocity = pCollidingBody->getAngularVelocity();
            LinearVelocity.mul4( m_kVelocityModifier );
            AngularVelocity.mul4( m_kVelocityModifier );
            LinearVelocity.add4( pBreakingBody->getLinearVelocity() );
            AngularVelocity.add4( pBreakingBody->getAngularVelocity() );

            hkpRigidBodyCinfo RigidBodyCinfo;
            RigidBodyCinfo.m_shape = pBrokenOffShape;
            RigidBodyCinfo.m_position = pBreakingBody->getPosition();
            RigidBodyCinfo.m_rotation = pBreakingBody->getRotation();
            RigidBodyCinfo.m_linearVelocity = LinearVelocity;
            RigidBodyCinfo.m_angularVelocity = AngularVelocity;
            RigidBodyCinfo.m_mass = m_kfMass;
            RigidBodyCinfo.m_solverDeactivation = hkpRigidBodyCinfo::SOLVER_DEACTIVATION_HIGH;
            RigidBodyCinfo.m_qualityType = HK_COLLIDABLE_QUALITY_DEBRIS;

            hkpInertiaTensorComputer::setShapeVolumeMassProperties(
                pBrokenOffShape, RigidBodyCinfo.m_mass, RigidBodyCinfo
                );

            static const f32 kfMaxInertiaRatio = 2000.0f;
            hkpInertiaTensorComputer::clipInertia( kfMaxInertiaRatio, RigidBodyCinfo );

            pNewBody = new hkpRigidBody( RigidBodyCinfo );

            //
            // If the original unbroken body is fixed, the colliding impulse is lost.  Just apply the
            //  impulse to the new piece
            //
            if ( pBreakingBody->isFixedOrKeyframed() )
            {
                hkReal BreakImpulse = pPointInfo->m_properties->m_maxImpulse;
                hkVector4 Impulse;

                hkReal BreakImpulseModifier = -BreakImpulse * 0.2f;

                Impulse.setMul4( BreakImpulseModifier, pPointInfo->m_contactPoint->getNormal() );
                pNewBody->applyPointImpulse( Impulse, pPointInfo->m_contactPoint->getPosition() );
            }

            //
            // Add to the list of broken off body parts.  Will be used later to extend a UObject.
            //
            ExtensionData ed;
            ed.pszName = reinterpret_cast<pcstr>(pBrokenOffShape->getUserData());
            ed.pBody = pNewBody;

            {
                //
                // Lock PhysicsSystem, KeysBrokenOff, and m_aBrokenOffParts.
                //
                std::lock_guard<std::mutex> lock( m_BrokenOffPartsSpinWait );

                //
                // Remove the shape from the break-off-parts utility.
                //
                hkpBreakOffPartsUtil::removeKeysFromListShape( pBreakingBody, &BrokenPieceKey, 1 );
                KeysBrokenOff.pushBack( BrokenPieceKey );

                //
                // Update the broken parts list.
                //
                m_aExtensions[ (void*)pBrokenOffShape ] = ed;
            }

            //
            // Inform the universal scene we want to extend an object.
            //
            PostChanges( System::Changes::Generic::ExtendObject );

            Result = HK_SUCCESS;
        }
    }

    return Result;
}


///////////////////////////////////////////////////////////////////////////////
// AddTrackCollision - Adds a collision to track and report
void
HavokPhysicsScene::AddTrackCollision(
    HavokPhysicsObject* pSubject,
    HavokPhysicsObject* pObserver
    )
{
    //
    // If the subject is already in the list the just add the new tracker request, otherwise
    //  create a new one.
    //
    CollisionTrackerIt itSubject = m_CollisionTracker.find( pSubject );

    if ( itSubject != m_CollisionTracker.end() )
    {
#ifdef _DEBUG
        //
        // Check for double registration.
        //
        for ( std::vector<HavokPhysicsObject*>::iterator it=itSubject->second.begin();
              it != itSubject->second.end(); it++ )
        {
            if ( *it == pObserver )
            {
                ASSERTMSG( False, "Already tracking the specified subject." );
            }
        }
#endif

        //
        // Add the info to the existing collection.
        //
        itSubject->second.push_back( pObserver );
    }
    else
    {
        //
        // Create a new info collection and add it to the subject collection.
        //
        std::vector<HavokPhysicsObject*> v;
        v.push_back( pObserver );

        m_CollisionTracker[ pSubject ] = v;
    }
}


///////////////////////////////////////////////////////////////////////////////
// RemoveTrackCollision - Removes a collision to track and report
void
HavokPhysicsScene::RemoveTrackCollision(
    HavokPhysicsObject* pSubject,
    HavokPhysicsObject* pObserver
    )
{
    //
    // Get the iterator and remove the observer.  Do nothing if the subject was never tracked.
    //
    CollisionTrackerIt itSubject = m_CollisionTracker.find( pSubject );

    if ( itSubject != m_CollisionTracker.end() )
    {
        for ( std::vector<HavokPhysicsObject*>::iterator it=itSubject->second.begin();
              it != itSubject->second.end(); it++ )
        {
            if ( *it == pObserver )
            {
                itSubject->second.erase( it );
                break;
            }
        }

        //
        // If there are no more observers, then remove the subject.
        //
        if ( itSubject->second.size() == 0 )
        {
            m_CollisionTracker.erase( itSubject );
        }
    }
}
