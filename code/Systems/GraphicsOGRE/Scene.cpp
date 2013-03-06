//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <OgreFontManager.h>
//standard
#include <cfloat>
#include <stdio.h>
//system
#include "Systems/GraphicsOGRE/Extras/MovableText.hpp"
#include "Systems/GraphicsOGRE/System.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/Task.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectCamera.hpp"
#include "Systems/GraphicsOGRE/ObjectLight.hpp"
#include "Systems/GraphicsOGRE/ObjectLightFire.hpp"
#include "Systems/GraphicsOGRE/ObjectMesh.hpp"
#include "Systems/GraphicsOGRE/ObjectMeshAnimated.hpp"
#include "Systems/GraphicsOGRE/ObjectParticles.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectChart.hpp"
#include "Systems/GraphicsOGRE/ObjectStatWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectWorkloadWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectCPUChart.hpp"
#include "Systems/GraphicsOGRE/ObjectParticleSystem.hpp"
#include "Systems/GraphicsOGRE/ObjectPagedGeometryLayer.hpp"


#define FONT_FILE_NAME "bluebold.ttf"
#define FONT_FOLDER "..//media//fonts"

#define POGREROOT       (reinterpret_cast<OGREGraphicsSystem*>(m_pSystem)->GetOGRERoot())

extern ManagerInterfaces       g_Managers;

// We use SM2.0 instancing. Since we do normal mapping on the instanced geometry, we 
// need to pass both the world matrix and the inverse world matrix for each instance.
// Given our limit of 256 float4 constants, we set the Objects per batch to 40.
// 40 float3x3 world matrices           = 120 float3/4 constants
// 40 float3x3 world inverse matrices   = 120 float3/4 constants
// we have used 240 float4 constants. The rest 16 are saved to pass other information
// to the instancing shader.
static const u32 maxObjectsPerBatch = 40;

// Prototypes
void ProcessObjects( void* Data );

static float StaticTerrainHeightScene(const float a, const float b, void *userData)
{
    UNREFERENCED_PARAM(userData);
    return ((OGREGraphicsScene*)userData)->getTerrainHeightScene( a, b, NULL );
}

static const u32   UpdateGrainSize = 120;


pcstr OGREGraphicsScene::sm_kapszPropertyNames[] =
{
    "ResourceLocation", "DelResourceLocation",
    "AmbientLight", "Shadows", "ShadowColor", 
    "DrawBoundingBox", "ShowNormals", "ShowTangents",
    "UseStaticGeom", "UseInstancedGeom", "FogColor", "Fog", "Font",
    "PagedGeometry", "PagedGeometryTerrain", "PagedGeometryTerrainOffset"
};

const Properties::Property OGREGraphicsScene::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_ResourceLocation ],
                          VALUE4( Properties::Values::String,
                                  Properties::Values::String,
                                  Properties::Values::String,
                                  Properties::Values::Boolean ),
                          Properties::Flags::Valid | Properties::Flags::Multiple | Properties::Flags::InitOnly,
                          "Path", "Type", "Group", "Recursive",
                          "", "", "", 0 ),

    Properties::Property( sm_kapszPropertyNames[ Property_DelResourceLocation ],
                          VALUE1x2( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::Multiple | Properties::Flags::InitOnly,
                          "Path", "Group", NULL, NULL,
                          "", "" ),

    Properties::Property( sm_kapszPropertyNames[ Property_AmbientLight ],
                          Properties::Values::Color3,
                          Properties::Flags::Valid,
                          "R", "G", "B", NULL,
                          Base::Color3::Black ),
    Properties::Property( sm_kapszPropertyNames[ Property_Shadows ],
                          VALUE1x2( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0, 0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_ShadowColor ],
                          Properties::Values::Color3,
                          Properties::Flags::Valid,
                          "R", "G", "B", NULL,
                          Base::Color3::Black ),
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
    Properties::Property( sm_kapszPropertyNames[ Property_UseStaticGeom ],
                          Properties::Values::Boolean,
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          1 ),
    Properties::Property( sm_kapszPropertyNames[ Property_UseInstancedGeom ],
                          Properties::Values::Boolean,
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_FogColor ],
                          Properties::Values::Color3,
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "R", "G", "B", NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_Fog ],
                          VALUE4(
                                Properties::Values::Int32,
                                Properties::Values::Float32,
                                Properties::Values::Float32,
                                Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Mode", "Density", "Start", "Stop",
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_Font ],
                          VALUE1x2(Properties::Values::String),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "FontPath", "FontName", NULL, NULL,
                          "", "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_PagedGeometry ],
                          VALUE4(Properties::Values::String,
                                 Properties::Values::String,
                                 Properties::Values::Float32,
                                 Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Heightmap", "ResourceGroup", "Pagesize", "Drawdistance",
                          "", "", 100.0, 1000.0),
    Properties::Property( sm_kapszPropertyNames[ Property_PagedGeometryTerrain ],
                          VALUE1x3(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Width", "Length", "Height", NULL,
                          100.0, 100.0, 100.0),
    Properties::Property( sm_kapszPropertyNames[ Property_PagedGeometryTerrainOffset ],
                          VALUE1x3(Properties::Values::Float32),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          "Width", "Length", "Height", NULL,
                          0.0, 0.0, 0.0),
};

OGREGraphicsScene::OGREGraphicsScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )
    , m_pTask( NULL )
    , m_pSceneManager( NULL )
    , m_pRootNode( NULL )
    , m_bUseStaticGeom( 1 )
    , m_bUseInstancedGeom( 0 )
    , m_FogColor( 1.0f, 1.0f, 1.0f, 1.0f)
    , m_FogMode( Ogre::FOG_NONE )
    , m_ExpDensity( .001f )
    , m_LinearStart( 0.0f )
    , m_LinearEnd( 1.0f )
    , m_pPagedGeometry( NULL )
    , m_pGrassLoader( NULL )
    , m_bParallelize(False)
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );
}


OGREGraphicsScene::~OGREGraphicsScene(
    void
    )
{
    SAFE_DELETE( m_pTask );
    SAFE_DELETE( m_pPagedGeometry );
    SAFE_DELETE( m_pGrassLoader );

    ASSERT( m_pSceneManager != NULL );
    POGREROOT->destroySceneManager( m_pSceneManager );
}


void 
OGREGraphicsScene::Update( 
       f32 DeltaTime 
       )
{
    Bool bPaused = g_Managers.pEnvironment->Runtime().GetStatus() ==
                   IEnvironment::IRuntime::Status::Paused;
    m_bPause = bPaused;
    m_fDeltaTime = DeltaTime;
    
    Ogre::WindowEventUtilities::messagePump();

    if( m_bUseInstancedGeom )
    {
        std::map< std::string, std::vector<Ogre::InstancedGeometry::InstancedObject**> > mapObjArray;

        // Now build every Instanced Geometry group
        std::map< std::string, std::vector<Ogre::InstancedGeometry*> >::iterator itMap = m_InstancedGeoms.begin();
        for ( itMap = m_InstancedGeoms.begin(); itMap != m_InstancedGeoms.end(); itMap++ )
        {
            for( u32 i=0; i < itMap->second.size(); i++ )
            {
                Ogre::InstancedGeometry* pInstancedGeom = itMap->second[ i ];

                Ogre::InstancedGeometry::BatchInstance* pBatchInst = pInstancedGeom->getBatchInstanceIterator().getNext();

                Ogre::InstancedGeometry::InstancedObject** tempArr;                
                Ogre::InstancedGeometry::BatchInstance::InstancedObjectIterator bit = pBatchInst->getObjectIterator();
                int j = 0;
                while(bit.hasMoreElements())
                {
                    tempArr[j] = bit.getNext();
                    ++j;
                }               
                
                mapObjArray[ itMap->first ].push_back( tempArr );
            }
        }

        // Set Position for each entity
        ObjectsList::iterator it = m_Objects.begin();
        for ( it = m_Objects.begin(); it != m_Objects.end(); it++ )
        {
            // Only look for Mesh type Objects
            if( ((*it)->GetType() != OGREGraphicsObject::Type_Mesh))
            {
                continue;
            }

            OGREGraphicsObjectMesh* pObjMesh = dynamic_cast<OGREGraphicsObjectMesh*> (*it);
            std::string&            grpName  = pObjMesh->m_strStaticGrpName;              

            if( grpName.length() != 0 )
            {
                // This ref-count seems to be outdated. If it is replaced by bool
                // everything works correctly.
                if( pObjMesh->m_Dirty )
                {
                    Ogre::InstancedGeometry::InstancedObject** objArr;

                    objArr = mapObjArray[ grpName ][ pObjMesh->m_InstancedGeomIdx ];

                    objArr[ pObjMesh->m_ObjectIdxinInstGeom ]->setPositionAndOrientation( 
                                                        TOOGREVEC(pObjMesh->m_Position),
                                                        TOOGREQUAT(pObjMesh->m_Orientation) );

                    pObjMesh->m_Dirty = False;
                }
            }
        }


        //clean up
        for ( itMap = m_InstancedGeoms.begin(); itMap != m_InstancedGeoms.end(); itMap++ )
        {
            for( u32 i=0; i < itMap->second.size(); i++ )
            {
                delete[] mapObjArray[ itMap->first ] [ i ];
            }
        }
    }

    u32         size = (u32)m_Objects.size();

    if (m_bParallelize && ( g_Managers.pTask != NULL ) && ( UpdateGrainSize < size ))
    {
        g_Managers.pTask->ParallelFor( m_pTask, UpdateCallback, this, 0, size, UpdateGrainSize );
    }
    else
    {
        ProcessRange( 0, size );
    }

    //
    // Paged Geometry needs to be updated every frame
    //
    if (m_pPagedGeometry) {
        m_pPagedGeometry->update();
    }

} // OGREGraphicsScene::Update



System::Type
OGREGraphicsScene::GetSystemType(
    void
    )
{
    return System::Types::Graphics;
}



Ogre::String buildInstancedMaterial(const Ogre::String &originalMaterialName)
{
    // already instanced ?
    if (Ogre::StringUtil::endsWith (originalMaterialName, "/instanced"))
    {
        return originalMaterialName;
    }

    Ogre::MaterialPtr originalMaterial = Ogre::MaterialManager::getSingleton().getByName(originalMaterialName);

    const Ogre::String instancedMaterialName ( originalMaterialName + "/Instanced" );
    Ogre::MaterialPtr  instancedMaterial = Ogre::MaterialManager::getSingleton().getByName( instancedMaterialName );

    // already exists ?
    if (instancedMaterial.isNull())
    {
        instancedMaterial = originalMaterial->clone(instancedMaterialName);
        instancedMaterial->load();
        Ogre::Technique::PassIterator pIt = instancedMaterial->getBestTechnique ()->getPassIterator();
        while (pIt.hasMoreElements())
        {

            Ogre::Pass * const pPass = pIt.getNext();

            pPass->setVertexProgram("Instancing");
            Ogre::GpuProgramParametersSharedPtr pParams = pPass->getVertexProgramParameters();
        }
    }
    instancedMaterial->load();
    return instancedMaterialName;
}


void 
OGREGraphicsScene::GlobalSceneStatusChanged( GlobalSceneStatus Status )
{
    // Create the StaticGeom and InstancedGeom objects after we finish loading all
    // the separate graphic objects.
    if( Status == GlobalSceneStatus::PostLoadingObjects )
    {
        if( m_bUseStaticGeom )
        {
            // Check each object for Static options
            for ( ObjectsList::iterator it = m_Objects.begin(); it != m_Objects.end(); it++ )
            {
                // If Mesh is specified as part of a StaticGeom group
                if( ((*it)->GetType() == OGREGraphicsObject::Type_Mesh) &&
                    ((dynamic_cast<OGREGraphicsObjectMesh*>(*it)->m_strStaticGrpName).length() != 0) )
                {
                    OGREGraphicsObjectMesh* pObjMesh = dynamic_cast<OGREGraphicsObjectMesh*> (*it);

                    // Create Static Geometry if not already created
                    if( !m_StaticGeoms[ pObjMesh->m_strStaticGrpName ] )
                    {
                        m_StaticGeoms[ pObjMesh->m_strStaticGrpName ] = 
                            m_pSceneManager->createStaticGeometry( pObjMesh->m_strStaticGrpName );
                    }
                  
                    m_StaticGeoms[ pObjMesh->m_strStaticGrpName ]->addEntity( 
                                                pObjMesh->m_pEntity, 
                                                TOOGREVEC ( pObjMesh->m_Position ),
                                                TOOGREQUAT( pObjMesh->m_Orientation ),
                                                TOOGREVEC ( pObjMesh->m_Scale ) );
                    pObjMesh->m_pNode->detachAllObjects();
                    m_pSceneManager->destroySceneNode( pObjMesh->m_pNode->getName() );
                    pObjMesh->m_pNode = NULL;
                    m_pSceneManager->destroyEntity( pObjMesh->m_pEntity );
                    pObjMesh->m_pEntity = NULL;
                }
            }
                    
            
            // Now build every Static Geometry group
            std::map<std::string, Ogre::StaticGeometry*>::iterator itStat = m_StaticGeoms.begin();
            for ( itStat = m_StaticGeoms.begin(); itStat != m_StaticGeoms.end(); itStat++ )
            {
                Ogre::StaticGeometry* pStaticGeom = itStat->second;

                f32 size;
                size = FLT_MAX;
                pStaticGeom->setRegionDimensions (Ogre::Vector3(size, size, size));

                pStaticGeom->setCastShadows( true );

                // Build should be the last call after setting the params for each StaticGeom group
                pStaticGeom->build();
            }
        }  

        if( m_bUseInstancedGeom )
        {

            std::map<std::string, u32> groupCount;

            // Check each object for Static options
            for ( ObjectsList::iterator it = m_Objects.begin(); it != m_Objects.end(); it++ )
            {
                // Only look for Mesh type Objects
                if( ((*it)->GetType() != OGREGraphicsObject::Type_Mesh)) 
                {
                    continue;
                }

                OGREGraphicsObjectMesh* pObjMesh = dynamic_cast<OGREGraphicsObjectMesh*> (*it);
                std::string&            grpName  = pObjMesh->m_strStaticGrpName;              
                Ogre::Entity*           pEnt     = pObjMesh->m_pEntity;

                if( grpName.length() != 0 )
                {
                    //setupInstancedMaterialToEntity
                    for (u32 i = 0; i < pEnt->getNumSubEntities(); ++i)
                    {
                        Ogre::SubEntity* se = pEnt->getSubEntity(i);
                        Ogre::String materialName = se->getMaterialName();

                        se->setMaterialName( buildInstancedMaterial( materialName ) );
                    }

                    if( groupCount.find( grpName ) == groupCount.end() )
                        groupCount[ grpName ] = 0;


                    // Create Instanced Geometry if neeeded
                    pObjMesh->m_InstancedGeomIdx = groupCount[ grpName ] / maxObjectsPerBatch;
                    pObjMesh->m_ObjectIdxinInstGeom = groupCount[ grpName ] % maxObjectsPerBatch;

                    if( !(groupCount[ grpName ] % maxObjectsPerBatch) )
                    {
                        char groupIndex[4];
                        
                        sprintf(groupIndex,"%d",pObjMesh->m_InstancedGeomIdx);
                        //_itoa_s( pObjMesh->m_InstancedGeomIdx, groupIndex, 3, 10 );
                        
                        Ogre::InstancedGeometry* pIG = m_pSceneManager->createInstancedGeometry( grpName + groupIndex ) ;
                        m_InstancedGeoms[ grpName ].push_back( pIG );
                        pIG->setProvideWorldInverses( true );
                    }
                    
                    m_InstancedGeoms[ pObjMesh->m_strStaticGrpName ][groupCount[ grpName ] / maxObjectsPerBatch]->addEntity( 
                                                pObjMesh->m_pEntity, 
                                                TOOGREVEC ( pObjMesh->m_Position ),
                                                TOOGREQUAT( pObjMesh->m_Orientation ),
                                                TOOGREVEC ( pObjMesh->m_Scale ) );
                    pObjMesh->m_pNode->detachAllObjects();
                    m_pSceneManager->destroySceneNode( pObjMesh->m_pNode->getName() );
                    pObjMesh->m_pNode = NULL;
                    m_pSceneManager->destroyEntity( pObjMesh->m_pEntity );
                    pObjMesh->m_pEntity = NULL;

                    groupCount[ grpName ]++;
                }
            }
                    
            
            std::map< std::string, std::vector<Ogre::InstancedGeometry::InstancedObject**> > mapObjArray;

            // Now build every Instanced Geometry group
            std::map< std::string, std::vector<Ogre::InstancedGeometry*> >::iterator itMap = m_InstancedGeoms.begin();
            for ( itMap = m_InstancedGeoms.begin(); itMap != m_InstancedGeoms.end(); itMap++ )
            {
                for( u32 i=0; i < itMap->second.size(); i++ )
                {
                    Ogre::InstancedGeometry* pInstancedGeom = itMap->second[ i ];
                    pInstancedGeom->setCastShadows( true );
                    pInstancedGeom->setProvideWorldInverses( true );
                    pInstancedGeom->build();
                    pInstancedGeom->setVisible( true );

                    // Prepare to set positions
                    // get all instanced objects as arrays
                    Ogre::InstancedGeometry::BatchInstance* pBatchInst = pInstancedGeom->getBatchInstanceIterator().getNext();

                    Ogre::InstancedGeometry::InstancedObject** tempArr;                
                    Ogre::InstancedGeometry::BatchInstance::InstancedObjectIterator bit = pBatchInst->getObjectIterator();
                    int j = 0;
                    while(bit.hasMoreElements())
                    {
                        tempArr[j] = bit.getNext();
                        ++j;
                    }               
                    
                    mapObjArray[ itMap->first ].push_back( tempArr );
                }
            }


            // Set Position for each entity
            //std::list<OGREGraphicsObject*>::iterator it = m_Objects.begin();
            for ( ObjectsList::iterator it = m_Objects.begin(); it != m_Objects.end(); it++ )
            {
                // Only look for Mesh type Objects
                if( ((*it)->GetType() != OGREGraphicsObject::Type_Mesh))
                {
                    continue;
                }

                OGREGraphicsObjectMesh* pObjMesh = dynamic_cast<OGREGraphicsObjectMesh*> (*it);
                std::string&            grpName  = pObjMesh->m_strStaticGrpName;              

                if( grpName.length() != 0 )
                {
                    Ogre::InstancedGeometry::InstancedObject** objArr;

                    objArr = mapObjArray[ grpName ][ pObjMesh->m_InstancedGeomIdx ];

                    objArr[ pObjMesh->m_ObjectIdxinInstGeom ]->setPositionAndOrientation( 
                                                        TOOGREVEC(pObjMesh->m_Position),
                                                        TOOGREQUAT(pObjMesh->m_Orientation) );
                }
            }


            //clean up
            for ( itMap = m_InstancedGeoms.begin(); itMap != m_InstancedGeoms.end(); itMap++ )
            {
                for( u32 i=0; i < itMap->second.size(); i++ )
                {
                    delete[] mapObjArray[ itMap->first ] [ i ];
                }
            }
        }
    }
}


//
// The paged geometry system will call this function with x and z coordinates. We need to
// somehow calculate the appropriate height value (y) at that point and return it.
//
float OGREGraphicsScene::getTerrainHeightScene(const float a, const float b, void *userData)
{
    UNREFERENCED_PARAM(userData);

    float x = a;
    float y = b;

    //
    // Calculate the pixel in the heightmap corresponding to the (x,z) point in world space.
    //
    u32 heightMapWidth = (u32) m_HeightMapImage.getWidth();
    u32 heightMapHeight = (u32) m_HeightMapImage.getHeight();

    x += (m_fTerrainWidth / 2.0f);
    y += (m_fTerrainLength / 2.0f);

    x = x / m_fTerrainWidth;
    y = y / m_fTerrainLength;

    x *= (float)heightMapWidth;
    y *= (float)heightMapHeight;

    //
    // Clamp the min and max values as when the camera is near the edge of the terrain we
    // will be positioning objects that are outside the heightmap boundaries.
    //
    if (x >= heightMapWidth) {
        x = (float)heightMapWidth - 1.0f;
    } else if (x < 0.0f) {
        x = 0.0f;
    }

    if (y >= heightMapHeight) {
        y = (float)heightMapHeight - 1.0f;
    } else if (y < 0.0f) {
        y = 0.0f;
    }

    //
    // The heightmap is a grayscale image so any color will do. We also need to account
    // for the fact that the terrain may be shifted up or down.
    //
    Ogre::ColourValue cv = m_HeightMapImage.getColourAt((int)x, (int)y, 0); 
    y = (cv.g * m_fTerrainHeight) + m_fTerrainHeightOffset;


    return y;
}

Error
OGREGraphicsScene::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err = Errors::Success;

    ASSERT( !m_bInitialized );

    //
    // Intialize the scene manager.
    //
    m_pSceneManager = POGREROOT->createSceneManager( Ogre::ST_GENERIC );
    ASSERT( m_pSceneManager != NULL );

    if (!m_pSceneManager)
    {
        return Errors::Failure;
    }

    //
    // Get the root node.
    //
    m_pRootNode = m_pSceneManager->getRootSceneNode();
    ASSERT( m_pRootNode != NULL );
    m_pRootNode->hideBoundingBox( true );

    //
    // Create the task for rendering all of this.
    //
    m_pTask = new OGREGraphicsTask( this, POGREROOT );
    ASSERT( m_pTask != NULL );

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    //
    // Read the properties.
    //
    Ogre::ResourceGroupManager* pResourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();

    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid &&
            it->GetFlags() & Properties::Flags::InitOnly)
        {
            std::string sName = it->GetName();
            if ( sName == sm_kapszPropertyNames[ Property_ResourceLocation ] )
            {
                pcstr pszName = it->GetStringPtr( 0 );
                pcstr pszLocationType = it->GetStringPtr( 1 );
                pcstr pszResourceGroup = it->GetStringPtr( 2 );
                Bool  bRecursive = it->GetBool( 3 ); 

                pResourceGroupManager->addResourceLocation(
                    pszName, pszLocationType, pszResourceGroup, (bRecursive == True)
                    );

                pResourceGroupManager->loadResourceGroup( pszResourceGroup );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_DelResourceLocation ] )
            {
                pcstr pszName = it->GetStringPtr( 0 );
                pcstr pszResourceGroup = it->GetStringPtr( 1 );

                pResourceGroupManager->unloadResourceGroup( pszResourceGroup );
                pResourceGroupManager->clearResourceGroup( pszResourceGroup );
                pResourceGroupManager->removeResourceLocation(
                    pszName, pszResourceGroup );

                pResourceGroupManager->loadResourceGroup( pszResourceGroup );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_UseStaticGeom ] )
            {
                m_bUseStaticGeom = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_UseInstancedGeom ] )
            {
                m_bUseInstancedGeom = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_FogColor ] )
            {
                m_FogColor.r = it->GetColor3().r;
                m_FogColor.g = it->GetColor3().g;
                m_FogColor.b = it->GetColor3().b;
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Fog ] )
            {
                i32 mode = it->GetInt32(0);
                switch ( mode )
                {
                case 1:
                    m_FogMode = Ogre::FOG_EXP;
                    break;
                case 2:
                    m_FogMode = Ogre::FOG_EXP2;
                    break;
                case 3:
                    m_FogMode = Ogre::FOG_LINEAR;
                    break;
                default:
                    m_FogMode = Ogre::FOG_NONE;
                    break;
                }
                m_ExpDensity  = it->GetFloat32(1);
                m_LinearStart = it->GetFloat32(2);
                m_LinearEnd   = it->GetFloat32(3);
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Font ] )
            {
                //-------------------------------------------------------------------------
                // Here is where we load the "default" font into the ogre resource system. We don't need
                // to keep any references to the resource around as it will be accessed by name by anyone
                // who needs the resource.
                //
                std::string sPath = it->GetString(0);
                std::string sFile = it->GetString(1);
                Ogre::ResourceGroupManager *pResGroupMgr = Ogre::ResourceGroupManager::getSingletonPtr();
                pResGroupMgr->addResourceLocation(sPath, "FileSystem");
                Ogre::FontManager &fontManager = Ogre::FontManager::getSingleton();
                // The first parameter here is the name of the font resource.
                Ogre::ResourcePtr font = fontManager.create("MyFont", "General");
                font->setParameter("type", "truetype");
                font->setParameter("source", sFile);
                font->setParameter("size", "24");
                font->setParameter("resolution", "96");
                font->load();
            }
            //
            // Load the paged geometry information. This is broken up between three areas. Here, the camera,
            // and the individual layers in ObjectPagedGeometryLayer.cpp. Here, we initialize the system and
            // load general information that will be shared between all layers. In ObjectCamera the camera
            // needs to be associated with PagedGeometry. There can be multiple layers (e.g. grass, rocks, flowers)
            // so they are taken care of as their own objects.
            //
            else if ( sName == sm_kapszPropertyNames[ Property_PagedGeometry ] )
            {
                m_pPagedGeometry = new Forests::PagedGeometry();
                if (!m_pPagedGeometry)
                {
                    Err = Errors::Memory::OutOfMemory;
                    return Err;
                }
                m_pGrassLoader   = new Forests::GrassLoader(m_pPagedGeometry);
                if (!m_pGrassLoader)
                {
                    Err = Errors::Memory::OutOfMemory;
                    return Err;
                }

                m_sHeightmap     = it->GetString(0);
                m_sResourceGroup = it->GetStringPtr(1);
                m_fPageSize      = it->GetFloat32(2);
                m_fDrawDistance  = it->GetFloat32(3);

                m_pPagedGeometry->setPageSize(m_fPageSize);
                m_pPagedGeometry->setInfinite();
                m_pPagedGeometry->setPageLoader(m_pGrassLoader);
                m_pGrassLoader->setHeightFunction(StaticTerrainHeightScene, this);
                m_HeightMapImage.load(m_sHeightmap, m_sResourceGroup);
            }
            else if ( sName == sm_kapszPropertyNames[ Property_PagedGeometryTerrainOffset ] )
            {
                m_fTerrainWidth  = it->GetFloat32(0);
                m_fTerrainLength = it->GetFloat32(1);
                m_fTerrainHeight = it->GetFloat32(2);
            }
            else if ( sName == sm_kapszPropertyNames[ Property_PagedGeometryTerrain ] )
            {
                m_fTerrainWidthOffset  = it->GetFloat32(0);
                m_fTerrainLengthOffset = it->GetFloat32(1);
                m_fTerrainHeightOffset = it->GetFloat32(2);
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
    //
    // Intialize any resource groups added by the scene
    //
    pResourceGroupManager->initialiseAllResourceGroups();

    //
    // Set the properties for this scene.
    //
    SetProperties( Properties );

    
    //
    // Set some of the shadow properties even though shadows might be disabled.
    //
    m_pSceneManager->setShadowTextureCount( reinterpret_cast<OGREGraphicsSystem*>( m_pSystem )->m_uShadowTextureCount );
    m_pSceneManager->setShadowTextureSize( reinterpret_cast<OGREGraphicsSystem*>( m_pSystem )->m_uShadowTextureSize );

    m_pSceneManager->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::LiSPSMShadowCameraSetup()));
    m_pSceneManager->setShadowCasterRenderBackFaces(true);


    //-------------------------------------------------------------------------
    // This is where we "create" the overlay manager by retrieving its pointer. We
    // also create one overlay which is currently the only one in the system. All
    // overlay elements are currently attached to it.
    //
    m_pOverlayManager = Ogre::OverlayManager::getSingletonPtr();

    //-------------------------------------------------------------------------
    // Here we create the default overlay.
    //
    m_pOverlay = m_pOverlayManager->create("SmokeOverlay");
    m_pOverlay->show();

    //-------------------------------------------------------------------------
    // Here we set the fog properties for the scene
    //
    m_pSceneManager->setFog(m_FogMode, m_FogColor, m_ExpDensity, m_LinearStart, m_LinearEnd);

    Bool bUseThreads = g_Managers.pEnvironment->Variables().GetAsBool( "Graphics::Parallel", True );
    m_bParallelize = bUseThreads && ( g_Managers.pTask != NULL );

    return Err;
}


void
OGREGraphicsScene::GetProperties(
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
    if ( m_pSceneManager != NULL )
    {
        Ogre::ColourValue AmbientColor = m_pSceneManager->getAmbientLight();
        Base::Vector3 vAmbientColor(AmbientColor.r, AmbientColor.g, AmbientColor.b);
        Properties[ iProperty+Property_AmbientLight ].SetValue( vAmbientColor );

        Properties[ iProperty+Property_Shadows ].SetValue(
            0, m_pSceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE
            );

        Ogre::ColourValue ShadowColor = m_pSceneManager->getShadowColour();
        Base::Vector3 vShadowColor(ShadowColor.r, ShadowColor.g, ShadowColor.b);
        Properties[ iProperty+Property_AmbientLight ].SetValue( vShadowColor );

        Ogre::ColourValue FogColor = m_pSceneManager->getFogColour();
        Base::Vector3 vFogColor(FogColor.r, FogColor.g, FogColor.b);
        Properties[ iProperty + Property_FogColor ].SetValue( vFogColor );

        i32   mode         = m_pSceneManager->getFogMode();
        float density      = m_pSceneManager->getFogDensity();
        float start        = m_pSceneManager->getFogStart();
        float end          = m_pSceneManager->getFogEnd();
        Properties[ iProperty + Property_Fog ].SetValue(0, mode);
        Properties[ iProperty + Property_Fog ].SetValue(1, density);
        Properties[ iProperty + Property_Fog ].SetValue(2, start);
        Properties[ iProperty + Property_Fog ].SetValue(3, end);
    }
}


void
OGREGraphicsScene::SetProperties(
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

            
            if ( sName == sm_kapszPropertyNames[ Property_AmbientLight ] )
            {
                //
                // Set the scene's ambient light.
                //
                Ogre::ColourValue AmbientLight;

                AmbientLight.a = 1.0f;
                AmbientLight.r = it->GetColor3().r;
                AmbientLight.g = it->GetColor3().g;
                AmbientLight.b = it->GetColor3().b;

                m_pSceneManager->setAmbientLight( AmbientLight );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Shadows ] )
            {
                //
                // Enable/disable shadows.
                //
                m_pSceneManager->setShadowTechnique( (Ogre::ShadowTechnique) (it->GetInt32( 0 )) );
                m_pSceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
                m_pSceneManager->setShadowTextureSelfShadow( it->GetBool( 1 ) != 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_ShadowColor ] )
            {
                //
                // Set the scene's shadow color.
                //
                Ogre::ColourValue ShadowColor;

                ShadowColor = Ogre::ColourValue( it->GetColor3().r,
                                                 it->GetColor3().g,
                                                 it->GetColor3().b );
                m_pSceneManager->setShadowColour( ShadowColor );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_DrawBoundingBox ] )
            {
                //
                // Enable/disable the drawing of Object BoundingBoxes.
                //
                m_pSceneManager->showBoundingBoxes( it->GetBool( 0 ) == True );
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


pcstr*
OGREGraphicsScene::GetObjectTypes(
    void
    )
{
    return OGREGraphicsObject::sm_kapszTypeNames;
}


ISystemObject*
OGREGraphicsScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    ASSERT( m_bInitialized );

    OGREGraphicsObject* pObject = NULL;

    if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_Mesh ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectMesh( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_PointList ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectParticles( this, pszName );
    }
    else if ( strcmp( pszType,
                 OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_Light ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectLight( this, pszName );
    }
    else if ( strcmp( pszType,
                 OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_LightFire ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectLightFire( this, pszName );
    }
    else if ( strcmp( pszType,
                 OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_Camera ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectCamera( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_MeshAnimated ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectMeshAnimated( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_PointList ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectParticles( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_Window ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectWindow( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_StatWindow ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectStatWindow( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_Chart ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectChart( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_CPUChart ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectCPUChart( this, pszName );
    }
    else if ( strcmp( pszType,
                      OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_WorkloadWindow ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectWorkloadWindow( this, pszName );
    }
    else if ( strcmp( pszType,
        OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_ParticleSystem ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectParticleSystem( this, pszName );
    }
    else if ( strcmp( pszType,
        OGREGraphicsObject::sm_kapszTypeNames[ OGREGraphicsObject::Type_PagedGeometryLayer ] ) == 0 )
    {
        //
        // Create and return the OGRE graphics object.
        //
        pObject = new OGREGraphicsObjectLayer( this, pszName );
    }
    else
    {
        ASSERT( False );
    }

    //
    //  Store the newly created object for future access
    //
    if ( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }
    else
    {
        ASSERT( False );
    }

    return pObject;
}


Error
OGREGraphicsScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a OGREGraphicsObject so that the correct destructor will be called.
    //
    OGREGraphicsObject* pObject =
        reinterpret_cast<OGREGraphicsObject*>(pSystemObject);

    if ( pObject != NULL )
    {
        //
        // Remove the object from the list and delete it.
        //
        m_Objects.erase( std::find(m_Objects.begin(), m_Objects.end(), pObject) );

        delete pObject;
    }

    return Errors::Success;
}


ISystemTask*
OGREGraphicsScene::GetSystemTask(
    void
    )
{
    return m_pTask;
}


System::Changes::BitMask
OGREGraphicsScene::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::None;
}


const void*
OGREGraphicsScene::GetSystemChangeData(
    System::Change SystemChange
    )
{
    UNREFERENCED_PARAM( SystemChange );

    return NULL;
}

void OGREGraphicsScene::UpdateCallback( void *param, u32 begin, u32 end )
{
//    ASSERT ( dynamic_cast<OGREGraphicsScene*>(param));
    OGREGraphicsScene* pThis = static_cast<OGREGraphicsScene*>(param);

    pThis->ProcessRange( begin, end );
}


void OGREGraphicsScene::ProcessRange( u32 begin, u32 end )
{
    static const u32 nonPausable = 
              OGREGraphicsObject::Type_Camera |
              OGREGraphicsObject::Type_Window |
              OGREGraphicsObject::Type_StatWindow |
              OGREGraphicsObject::Type_Chart |
              OGREGraphicsObject::Type_CPUChart |
              OGREGraphicsObject::Type_WorkloadWindow;
        
    for ( size_t i = begin; i < end; ++i )
    {
        OGREGraphicsObject* pObject = m_Objects[i];

        // Update objects based on paused state
        if ( !m_bPause  || pObject->GetType() & nonPausable )
        {
            // Process this object
            pObject->Update( m_fDeltaTime );
        }
    }
}
