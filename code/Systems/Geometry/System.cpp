//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Geometry/Object.hpp"
#include "Systems/Geometry/Scene.hpp"
#include "Systems/Geometry/System.hpp"


GeometrySystem::GeometrySystem(
    void
    )
    : ISystem()
{
}


GeometrySystem::~GeometrySystem(
    void
    )
{
}


pcstr
GeometrySystem::GetName(
    void
    )
{
    return System::Names::Geometry;
}


System::Type
GeometrySystem::GetSystemType(
    void
    )
{
    return System::Types::Geometry;
}


Error
GeometrySystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


void
GeometrySystem::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


void
GeometrySystem::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
}


ISystemScene*
GeometrySystem::CreateScene(
    void
    )
{
    return new GeometryScene( this );
}


Error
GeometrySystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    GeometryScene* pScene = reinterpret_cast<GeometryScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}
