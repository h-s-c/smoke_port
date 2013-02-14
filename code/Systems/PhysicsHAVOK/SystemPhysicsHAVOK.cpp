// Base
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// System
#include "Systems/PhysicsHAVOK/System.hpp"


#if defined(COMPILER_MSVC)
#include <windows.h>

BOOL APIENTRY
DllMain(
    HMODULE hModule,
    DWORD Reason,
    LPVOID pReserved
    )
{
    switch ( Reason )
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
}
#endif

ManagerInterfaces   g_Managers;

void STDCALL
InitHavokPhysicsSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateHavokPhysicsSystem()
{
    return new HavokPhysicsSystem();
}


void STDCALL
DestroyHavokPhysicsSystem( ISystem* pSystem)
{
    delete reinterpret_cast<HavokPhysicsSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemPhysicsHAVOK = {
        &InitHavokPhysicsSystem,
        &CreateHavokPhysicsSystem,
        &DestroyHavokPhysicsSystem
    };
}
