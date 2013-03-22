#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/ProceduralTrees/Scene.hpp"
#include "Systems/ProceduralTrees/Task.hpp"
#include "Systems/ProceduralTrees/Object.hpp"


TreeTask::TreeTask(
    TreeScene* pScene
    )
    : ISystemTask( pScene )
    , m_pScene( pScene )
{
}


TreeTask::~TreeTask(
    void
    )
{
}


System::Type
TreeTask::GetSystemType(
    void
    )
{
    return System::Types::MakeCustom( 0 );
}


void
TreeTask::Update(
    f32 DeltaTime
    )
{
    //DBG_UNREFERENCED_PARAM( DeltaTime );

    std::list<TreeObject*>::iterator i, iend;
    i = m_pScene->m_Forest.begin();
    iend = m_pScene->m_Forest.end();
    for ( i;i!=iend;i++){
        (*i)->update(DeltaTime);
    }
    
    if ( DeltaTime <= 0.0f )
    {
        return;
    }


}


//void
//TreeTask::UpdateChanges(
//    int SpaceId
//    )
//{
//}

