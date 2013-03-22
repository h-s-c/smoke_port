#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systemss/ProceduralTrees/System.hpp"


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
#endif

ManagerInterfaces   g_Managers;

void STDCALL
InitProceduralTreesSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateProceduralTreesSystem()
{
    return new TreeSystem();
}


void STDCALL
DestroyProceduralTreesSystem( ISystem* pSystem)
{
    delete reinterpret_cast<TreeSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemProceduralTrees = {
        &InitProceduralTreesSystem,
        &CreateProceduralTreesSystem,
        &DestroyProceduralTreesSystem
    };
}
