// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//stdlib
#include <iostream>
//framework
#include "Framework/Universal.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////
// UScene
////////////////////////////////////////////////////////////////////////////////////////////////

UScene::UScene(
    IChangeManager* pSceneCCM,
    IChangeManager* pObjectCCM

    )
    : m_pSceneCCM( pSceneCCM )
    , m_pObjectCCM( pObjectCCM )
{
}


UScene::~UScene(
    void
    )
{
    //
    // Send "pre-destroying objects" message to the scene extensions.
    //
    for ( SystemScenesConstIt it=m_SystemScenes.begin(); it != m_SystemScenes.end(); it++ )
    {
        it->second->GlobalSceneStatusChanged(
            ISystemScene::GlobalSceneStatus::PreDestroyingObjects
            );
    }

    //
    // Get rid of all the links.
    //
    for (auto & old : m_ObjectLinks)
    {
        m_pObjectCCM->Unregister( old.pSubject, old.pObserver );
    }
    m_ObjectLinks.clear();

    //
    // Get rid of all the objects.
    //
    Objects Objs = m_Objects;
    for (auto & Obj : Objs)
    {
        DestroyObject( Obj );
    }
    m_Objects.clear();

    //
    // Send "post-destroying objects" message to the scene extensions then delete the scene.
    //
    SystemScenes SysScenes = m_SystemScenes;
    for (auto & SysScene : SysScenes)
    {
        ISystemScene* pSystemScene = SysScene.second;

        pSystemScene->GlobalSceneStatusChanged(
            ISystemScene::GlobalSceneStatus::PostDestroyingObjects
            );

        Unextend( pSystemScene );
    }
    m_SystemScenes.clear();
}


ISystemScene*
UScene::Extend(
    ISystem* pSystem
    )
{
    if ( pSystem == nullptr )
    {
        std::cerr << "pSystem == NULL" << std::endl;
    }

    //
    // Get the system's type.
    //
    System::Type SystemType = pSystem->GetSystemType();
    if ( m_SystemScenes.find( SystemType ) != m_SystemScenes.end() )
    {
        std::cerr << "The new scene to create for the selected system type already exists." << std::endl;
    }

    //
    // Have the system create it's scene.
    //
    ISystemScene* pScene = pSystem->CreateScene();
    if ( pScene != nullptr )
    {
        //
        // Add the scene to the collection.
        //
        m_SystemScenes[ SystemType ] = pScene;

        //
        // Register all changes made by the scene.
        //
        m_pSceneCCM->Register( pScene, System::Changes::Generic::All, this );
    }
    else
    {
        std::cerr << "pScene == NULL" << std::endl;
    }

    return pScene;
}


Error
UScene::Unextend(
    ISystemScene* pScene
    )
{
    if ( pScene == nullptr )
    {
        std::cerr << "pScene == NULL" << std::endl;
    }

    //
    // Get the system.
    //
    ISystem* pSystem = pScene->GetSystem();
    if ( pSystem == nullptr )
    {
        std::cerr << "pSystem == NULL" << std::endl;
    }

    //
    // Get the system's type.
    //
    System::Type SystemType = pSystem->GetSystemType();

    //
    // Find the system scene in the collection and remove it.
    //
    auto it = m_SystemScenes.find( SystemType );
    if ( it == m_SystemScenes.end() )
    {
        std::cerr << "it == m_SystemScenes.end()" << std::endl;
    }

    m_SystemScenes.erase( it );

    //
    // Unregister the scene from the CCM.
    //
    m_pSceneCCM->Unregister( pScene, this );

    //
    // Call the system to delete it's scene.
    //
    pSystem->DestroyScene( pScene );

    return Errors::Success;
}


UObject*
UScene::CreateObject(
    pcstr pszName
    )
{
    //
    // Create the new object.
    //
    auto   pObject = new UObject( this, pszName );
    if ( pObject == nullptr )
    {
        std::cerr << "pObject == NULL" << std::endl;
    }

    pObject->m_pObjectCCM = m_pObjectCCM; 

    //
    // Add the object to the collection.
    //
    m_Objects.push_back( pObject );

    //
    // Register the object with the scene's CCM.
    //
    m_pSceneCCM->Register(
        pObject, System::Changes::Generic::All | System::Changes::Geometry::All, this
        );

    return pObject;
}


Error
UScene::DestroyObject(
    UObject* pObject
    )
{
    if ( pObject == nullptr )
    {
        std::cerr << "pObject == NULL" << std::endl;
    }

    m_pSceneCCM->Unregister( pObject, this );
    m_Objects.remove( pObject );
    delete pObject;

    return Errors::Success;
}


UObject*
UScene::FindObject(
    pcstr pszName
    )
{
    UObject* pObject = nullptr;

    for ( UScene::ObjectsConstIt it=m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        if ( strcmp( pszName, (*it)->GetName() ) == 0 )
        {
            pObject = *it;
            break;
        }
    }

    return pObject;
}


void
UScene::CreateObjectLink(
    ISystemObject* pSubject,
    ISystemObject* pObserver
    )
{
    //
    // Register objects with the CCM.
    //
    System::Changes::BitMask Changes =
        pSubject->GetPotentialSystemChanges() & pObserver->GetDesiredSystemChanges();

    if ( Changes )
    {
        m_pObjectCCM->Register( pSubject, Changes, pObserver );

        //
        // Hold on to the list for unregistering later.
        //
        ObjectLinkData old = { pSubject, pObserver };
        m_ObjectLinks.push_back( old );

        //
        // Inform the link requester that the link has been established.
        //
        pSubject->PostChanges( System::Changes::Link );
    }
}


void
UScene::CreateObjectLink(
    UObject* pSubject,
    ISystemObject* pObserver
    )
{
    //
    // Register objects with the CCM.
    //
    System::Changes::BitMask Changes =
        pSubject->GetPotentialSystemChanges() & pObserver->GetDesiredSystemChanges();

    if ( Changes ) {
        m_pObjectCCM->Register( pSubject, Changes, pObserver );

        //
        // Hold on to the list for unregistering later.
        //
        ObjectLinkData old = { pSubject, pObserver };
        m_ObjectLinks.push_back( old );

        //
        // Inform the link requester that the link has been established.
        //
        pSubject->PostChanges( System::Changes::ParentLink );
    }
}


Error
UScene::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    switch ( ChangeType )
    {
    case System::Changes::Generic::CreateObject:
    {
        IGenericScene* pScene = dynamic_cast<IGenericScene*>(pSubject);

        IGenericScene::CreateObjectDataArray aObjectsToCreate;
        pScene->GetCreateObjects( aObjectsToCreate );

        for (auto & object : aObjectsToCreate)
        {
            if ( FindObject( object.pszName ) != nullptr )
            {
                std::cerr << "FindObject( it->pszName ) != NULL" << std::endl;
            }

            UObject* pObject = CreateObject( object.pszName );

            if ( pObject != nullptr )
            {
                System::Types::BitMask Type = 1;
                for ( u32 i=0; i < System::Types::MAX; i++ )
                {
                    if ( object.Types & Type )
                    {
                        auto ssIt = m_SystemScenes.find( Type );
                        if ( ssIt != m_SystemScenes.end() )
                        {
                            pObject->Extend( ssIt->second, nullptr );
                        }
                        else
                        {
                            std::cerr << "ssIt == m_SystemScenes.end()" << std::endl;
                        }
                    }

                    Type <<= 1;
                }
            }
            else
            {
                std::cerr << "pObject == NULL" << std::endl;
            }
        }
        break;
    }

    case System::Changes::Generic::DeleteObject:
    {
        IGenericScene* pScene = dynamic_cast<IGenericScene*>(pSubject);

        IGenericScene::DestroyObjectDataArray aObjectsToDestroy;
        pScene->GetDestroyObjects( aObjectsToDestroy );

        for (auto & object : aObjectsToDestroy)
        {
            UObject* pObject = FindObject( object );
            DestroyObject( pObject );
        }
        break;
    }

    case System::Changes::Generic::ExtendObject:
    {
        IGenericScene* pScene = dynamic_cast<IGenericScene*>(pSubject);

        IGenericScene::ExtendObjectDataArray aObjectsToExtend;
        pScene->GetExtendObjects( aObjectsToExtend );

        for (auto & object : aObjectsToExtend)
        {
            UObject* pObject = FindObject( object.pszName );

            if ( pObject != nullptr )
            {
                pObject->Extend( pScene->ExtendObject( object.pszName, object.pUserData ) );
            }
        }
        break;
    }

    case System::Changes::Generic::UnextendObject:
    {
        IGenericScene* pScene = dynamic_cast<IGenericScene*>(pSubject);

        std::vector<pcstr> aObjectsToUnextend;
        pScene->GetUnextendObjects( aObjectsToUnextend );

        for (auto & object : aObjectsToUnextend)
        {
            UObject* pObject = FindObject( object );
            if ( pObject != nullptr )
            {
                pObject->Unextend( pScene->UnextendObject( object )->GetSystemScene() );
            }
        }
        break;
    }
    }

    return Errors::Success;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// UObject
////////////////////////////////////////////////////////////////////////////////////////////////

UObject::UObject(
    UScene* pScene,
    pcstr pszName
    )
    : m_pScene( pScene )
    , m_pGeometryObject( nullptr )
    , m_pGraphicsObject( nullptr )
{
    //
    // Copy the name.
    //
    SetName( pszName );
}


UObject::~UObject(
    void
    )
{
    //
    // Remove all the extensions.
    //
    SystemObjects SysObjs = m_ObjectExtensions;
    for (auto & SysObj : SysObjs)
    {
        Unextend( SysObj.second->GetSystemScene() );
    }
    m_ObjectExtensions.clear();
}


ISystemObject*
UObject::Extend(
    ISystemScene* pSystemScene,
    pcstr pszSystemObjectType
    )
{
    if ( pSystemScene == nullptr )
    {
        std::cerr << "pSystemScene == NULL" << std::endl;
    }
    if ( m_ObjectExtensions.find( pSystemScene->GetSystemType() ) != m_ObjectExtensions.end() )
    {
        std::cerr << "m_ObjectExtensions.find( pSystemScene->GetSystemType() ) != m_ObjectExtensions.end()" << std::endl;
    }

    ISystemObject* pSystemObject = nullptr;

    //
    // Create the system object.
    //
    pSystemObject = pSystemScene->CreateObject( m_sName.c_str(), pszSystemObjectType );
    if ( pSystemObject == nullptr )
    {
        std::cerr << "pSystemObject == NULL" << std::endl;
    }
    else
    {
        Extend( pSystemObject );
    }
    return pSystemObject;
}


Bool
UObject::Extend(
    ISystemObject* pSystemObject
    )
{
    Bool bSuccess = False;
    
    if ( pSystemObject == nullptr )
    {
        std::cerr << "pSystemObject == NULL" << std::endl;
    }
    else if ( m_ObjectExtensions.find( pSystemObject->GetSystemType() ) == m_ObjectExtensions.end() )
    {
        //
        // Set this as the parent.
        //
        pSystemObject->SetParentObject( this );

        //
        // Get the changes this object will make and is looking for.
        //
        System::Changes::BitMask SysObjPotentialChanges =
            pSystemObject->GetPotentialSystemChanges();
        System::Changes::BitMask SysObjDesiredChanges =
            pSystemObject->GetDesiredSystemChanges();;

        if ( SysObjPotentialChanges & (System::Changes::Geometry::All |
                                       System::Changes::Graphics::AABB) )
        {
            //
            // Have the UObject watch for all the geometry and AABB changes this system makes.
            //
            m_pObjectCCM->Register(
                pSystemObject,
                System::Changes::Geometry::All | System::Changes::Graphics::AABB,
                this );
        }

        //
        // Register each object with scenes that care about the object's changes.
        //
        UScene::SystemScenes pScenes = m_pScene->GetSystemScenes();
        for (auto & pScenes_it : pScenes)
        {
            ISystemScene* pScene = pScenes_it.second;
            if ( pSystemObject->GetPotentialSystemChanges() & pScene->GetDesiredSystemChanges() )
            {
                m_pObjectCCM->Register( pSystemObject, pScene->GetDesiredSystemChanges(), pScene );
            }
        }

        //
        // Register each of the systems with each other.
        //
        System::Changes::BitMask Changes = pSystemObject->GetDesiredSystemChanges();
        for (auto & object : m_ObjectExtensions)
        {
            ISystemObject* pObj = object.second;
            if ( pObj->GetPotentialSystemChanges() & SysObjDesiredChanges )
            {
                m_pObjectCCM->Register( pObj, Changes, pSystemObject );
            }
            if ( SysObjPotentialChanges & pObj->GetDesiredSystemChanges() )
            {
                m_pObjectCCM->Register( pSystemObject, pObj->GetDesiredSystemChanges(), pObj );
            }
        }

        //
        // Add the system object to the list.
        //
        System::Type SystemType = pSystemObject->GetSystemType();

        m_ObjectExtensions[ SystemType ] = pSystemObject;

        //
        // Set up the speed path for the geometry and graphics objects.
        //
        if ( SystemType == System::Types::Geometry )
        {
            m_pGeometryObject = reinterpret_cast<IGeometryObject*>(pSystemObject);
            if ( m_pGeometryObject == nullptr )
            {
                std::cerr << "UObject::Extend - m_pGeometryObject == NULL" << std::endl;
                return False;
            }
        }
        else if ( SystemType == System::Types::Graphics )
        {
            m_pGraphicsObject = reinterpret_cast<IGraphicsObject*>(pSystemObject);
            if ( m_pGraphicsObject == nullptr )
            {
                std::cerr << "UObject::Extend - m_pGraphicsObject == NULL" << std::endl;
                return False;
            }
        }

        bSuccess = True;
    }

    return bSuccess;
}


void
UObject::Unextend(
    ISystemScene* pSystemScene
    )
{
    if ( pSystemScene  == nullptr )
    {
        std::cerr << "UObject::Unextend - pSystemScene  == NULL" << std::endl;
    }

    //
    // Get the iterator for the object.
    //
    System::Type SystemType = pSystemScene->GetSystem()->GetSystemType();

    auto SysObjIt = m_ObjectExtensions.find( SystemType );
    if ( SysObjIt == m_ObjectExtensions.end() )
    {
        std::cerr << "UObject::Unextend - The object to delete doesn't exist in the scene." << std::endl;
    }

    ISystemObject* pSystemObject = SysObjIt->second;

    //
    // Go through all the other systems and unregister them with this as subject and observer.
    //  The CCM should know if the objects are registered or not, and if not won't do anything.
    //
    for (auto & object : m_ObjectExtensions)
    {
        ISystemObject* pObj = object.second;

        //
        // Make sure he system object is not unregistering as an observer of itself.
        //
        if ( pSystemObject != pObj )
        {
            m_pObjectCCM->Unregister( pObj, pSystemObject );
            m_pObjectCCM->Unregister( pSystemObject, pObj );
        }
    }

    //
    // Unregister each object with scenes that cared about the object's changes.
    //
    UScene::SystemScenes pScenes = m_pScene->GetSystemScenes();
    for (auto & pScenes_it : pScenes)
    {
        ISystemScene* pScene = pScenes_it.second;

        if ( pSystemObject->GetPotentialSystemChanges() & pScene->GetDesiredSystemChanges() )
        {
            m_pObjectCCM->Unregister( pSystemObject,  pScene );
        }
    }

    //
    // Unregister the UObject as an observer of this system object.
    //
    m_pObjectCCM->Unregister( pSystemObject, this );

    //
    // Destroy the object.
    //
    pSystemScene->DestroyObject( pSystemObject );

    //
    // Remove the object from the map.
    //
    m_ObjectExtensions.erase( SysObjIt );
}


const UObject::SystemObjects&
UObject::GetExtensions(
    void
    )
{
    return m_ObjectExtensions;
}


ISystemObject*
UObject::GetExtension(
    System::Type SystemType
    )
{
    ISystemObject* pSystemObject = nullptr;

    SystemObjectsConstIt it = m_ObjectExtensions.find( SystemType );

    if ( it != m_ObjectExtensions.end() )
    {
        pSystemObject = it->second;
    }

    return pSystemObject;
}


System::Changes::BitMask
UObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Generic::All |
           System::Changes::Geometry::All |
           System::Changes::Graphics::AABB;
}


Error
UObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    UNREFERENCED_PARAM( pSubject );

    if ( ChangeType & (System::Changes::Generic::All |
                       System::Changes::Geometry::All |
                       System::Changes::Graphics::AABB) )
    {
        ChangeType &= (System::Changes::Generic::All |
                       System::Changes::Geometry::All |
                       System::Changes::Graphics::AABB);

        //
        // Post the pertinent changes made by the extension to the scene CCM.
        //
        PostChanges( ChangeType );
    }

    return Errors::Success;
}


const Base::Vector3*
UObject::GetPosition(
    void
    )
{
    if ( m_pGeometryObject == nullptr )
    {
        std::cerr << "UObject::GetPosition - m_pGeometryObject == NULL" << std::endl;
    }

    return m_pGeometryObject->GetPosition();
}


const Base::Quaternion*
UObject::GetOrientation(
    void
    )
{
    if ( m_pGeometryObject == nullptr )
    {
        std::cerr << "UObject::GetOrientation - m_pGeometryObject == NULL" << std::endl;
    }

    return m_pGeometryObject->GetOrientation();
}


const Base::Vector3*
UObject::GetScale(
    void
    )
{
    if ( m_pGeometryObject == nullptr )
    {
        std::cerr << "UObject::GetScale - m_pGeometryObject == NULL" << std::endl;
    }

    return m_pGeometryObject->GetScale();
}


u32 
UObject::GetSubMeshCount(
    void
    )
{
    std::cerr << "UObject::GetSubMeshCount - Not applicable for UObject." << std::endl;
    return 0;
}


u32
UObject::GetIndexDeclaration(
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetIndexDeclaration - Not applicable for UObject." << std::endl;
    return 0;
}


u32
UObject::GetVertexDeclarationCount(
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetVertexDeclarationCount - Not applicable for UObject." << std::endl;
    return 0;
}


void
UObject::GetVertexDeclaration(
    Out VertexDecl::Element* pVertexDecl,
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetVertexDeclaratio - Not applicable for UObject." << std::endl;
}


u32
UObject::GetIndexCount(
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetIndexCount - Not applicable for UObject." << std::endl;
    return 0;
}


u32
UObject::GetVertexCount(
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetVertexCount - Not applicable for UObject." << std::endl;
    return 0;
}


void
UObject::GetIndices(
    Out void* pIndices,
    In  u16 nSubMeshIndex
    )
{
    std::cerr << "UObject::GetIndices - Not applicable for UObject." << std::endl;
}


void
UObject::GetVertices(
    Out void* pVertices,
    In  u16 nSubMeshIndex,
    In  u16 nStreamIndex,
    In  u32 nVertexDeclCount,
    In  VertexDecl::Element* pVertexDecl
    )
{
    std::cerr << "UObject::GetVertices - Not applicable for UObject." << std::endl;
}


u32 
UObject::GetStreamsChanged(
    void
    )
{
    std::cerr << "UObject::GetStreamsChanged - Not applicable for UObject." << std::endl;
    return 0;
}


void
UObject::GetAABB(
    Out Base::Vector3& Min,
    Out Base::Vector3& Max
    )
{
    if ( m_pGraphicsObject == nullptr )
    {
        std::cerr << "UObject::GetAABB - m_pGraphicsObject == NULL" << std::endl;
    }

    return m_pGraphicsObject->GetAABB( Min, Max );
}
