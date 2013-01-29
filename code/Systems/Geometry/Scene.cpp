//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Geometry/Object.hpp"
#include "Systems/Geometry/Scene.hpp"


GeometryScene::GeometryScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )
{
}


GeometryScene::~GeometryScene(
    void
    )
{
    //
    // Free all the remaining objects.
    //
    for ( std::list<GeometryObject*>::iterator it=m_Objects.begin();
          it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();
}


System::Type
GeometryScene::GetSystemType(
    void
    )
{
    return System::Types::Geometry;
}


Error
GeometryScene::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


void
GeometryScene::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


void
GeometryScene::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}

    
pcstr*
GeometryScene::GetObjectTypes(
    void
    )
{
    return NULL;
}


ISystemObject*
GeometryScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    UNREFERENCED_PARAM( pszName );
    UNREFERENCED_PARAM( pszType );

    ASSERT( m_bInitialized );
    ASSERT( pszType == NULL );

    //
    // Create the object and add it to the object list.
    //
    GeometryObject* pObject = new GeometryObject( this );

    if ( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    return pObject;
}


Error
GeometryScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a GeometryObject so that the correct destructor will be called.
    //
    GeometryObject* pObject = reinterpret_cast<GeometryObject*>(pSystemObject);

    //
    // Remove the object from the list and delete it.
    //
    m_Objects.remove( pObject );

    SAFE_DELETE( pSystemObject );

    return Errors::Success;
}


ISystemTask*
GeometryScene::GetSystemTask(
    void
    )
{
    return NULL;
}


System::Changes::BitMask
GeometryScene::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::None;
}


const void*
GeometryScene::GetSystemChangeData(
    System::Change SystemChange
    )
{
    UNREFERENCED_PARAM( SystemChange );

    return NULL;
}
