#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/PhysicsHAVOK/System.hpp"

#include <Common/Base/keycode.cxx>
#include <Common/Base/Config/hkProductFeatures.cxx>

ManagerInterfaces   g_Managers;

void 
InitHavokPhysicsSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* 
CreateHavokPhysicsSystem()
{
    return new HavokPhysicsSystem();
}


void 
DestroyHavokPhysicsSystem( ISystem* pSystem)
{
    delete reinterpret_cast<HavokPhysicsSystem*>( pSystem );
}

extern "C" 
{
    COMPILER_DLLEXPORT struct SystemFuncs SystemPhysicsHAVOK = {
        &InitHavokPhysicsSystem,
        &CreateHavokPhysicsSystem,
        &DestroyHavokPhysicsSystem
    };
}
