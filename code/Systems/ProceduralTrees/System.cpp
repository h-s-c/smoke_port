#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "System/ProceduralTrees/System.hpp"
#include "System/ProceduralTrees/Scene.hpp"


TreeSystem::TreeSystem(
    void
    )
    : ISystem()
{
}


TreeSystem::~TreeSystem(
    void
    )
{

}


pcstr
TreeSystem::GetName(
    void
    )
{
    return "ProceduralTrees";
}


System::Type
TreeSystem::GetSystemType(
    void
    )
{
    return System::Types::MakeCustom( 0 );
}


Error
TreeSystem::Initialize(
    Properties::Array Properties
    )
{
    m_bInitialized = True;

    return Errors::Success;
}


void
TreeSystem::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


void
TreeSystem::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}


ISystemScene*
TreeSystem::CreateScene(
    void
    )
{
    return new TreeScene( this );
}


Error
TreeSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    TreeScene* pScene = reinterpret_cast<TreeScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}
