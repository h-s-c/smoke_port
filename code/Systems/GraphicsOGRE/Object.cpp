//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/Task.hpp"
#include "Systems/GraphicsOGRE/ObjectParticleSystem.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"



#define POGREROOTNODE (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGRERootSceneNode())


pcstr OGREGraphicsObject::sm_kapszTypeNames[] =
{
    "Light", "LightFire", "Camera", "Mesh", "MeshAnimated", "PointList", "Window", "StatWindow", "Chart", "CPUChart", "WorkloadWindow", 
    "ParticleSystem", "PagedGeometryLayer",
    nullptr
};


OGREGraphicsObject::OGREGraphicsObject(
    ISystemScene* pSystemScene,
    pcstr pszName
    )
    : ISystemObject( pSystemScene, pszName )
{
    m_pszName = pszName;
}


OGREGraphicsObject::~OGREGraphicsObject(
    void
    )
{
    POGREROOTNODE->removeChild( m_pNode );
}

void 
OGREGraphicsObject::Update( 
    f32 DeltaTime 
    )
{
    UNREFERENCED_PARAM( DeltaTime );
    return;
}


System::Type
OGREGraphicsObject::GetSystemType(
    void
    )
{
    return System::Types::Graphics;
}


Error
OGREGraphicsObject::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Create the scene node for this object.
    //
    m_pNode = POGREROOTNODE->createChildSceneNode( std::string( m_pszName ) + "_SceneNode" );
    ASSERT( m_pNode != NULL );

    return Errors::Success;
}


System::Types::BitMask
OGREGraphicsObject::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Geometry::Position |
           System::Changes::Geometry::Orientation |
           System::Changes::Geometry::Scale |
           System::Changes::Graphics::GUI;
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


inline
void UpdateGeometry( Ogre::SceneNode* pNode, System::Changes::BitMask ChangeType, IGeometryObject* pGeometryObject )
{
    if ( ChangeType & System::Changes::Geometry::Position )
    {
        const Base::Vector3& Position = *pGeometryObject->GetPosition();
        pNode->setPosition( Position.x, Position.y, Position.z );
    }

    if ( ChangeType & System::Changes::Geometry::Orientation )
    {
        const Base::Quaternion& Orientation = *pGeometryObject->GetOrientation();
        pNode->setOrientation( Orientation.w, Orientation.x, Orientation.y, Orientation.z );
    }

    if ( ChangeType & System::Changes::Geometry::Scale )
    {
        const Base::Vector3& Scale = *pGeometryObject->GetScale();
        pNode->setScale(Scale.x, Scale.y, Scale.z );
    }
}


Error
OGREGraphicsObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    if ( !m_pNode )
    {
        // StaticGeom and InstancedGeom objects are abstract groupings and 
        // are not globally attached to any scene node
        return Errors::Success;
    }

    if ( ChangeType & (System::Changes::Geometry::Position |
                       System::Changes::Geometry::Orientation |
                       System::Changes::Geometry::Scale)
       )
    {
        IGeometryObject* pGeometryObject = dynamic_cast<IGeometryObject*>(pSubject);

        if ( NeedsLocking(m_pNode) )
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            UpdateGeometry( m_pNode, ChangeType, pGeometryObject );
        }
        else
        {
            UpdateGeometry( m_pNode, ChangeType, pGeometryObject );
        }
    }

    return Errors::Success;
}


System::Changes::BitMask
OGREGraphicsObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::None;
}

