//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interfaces
#include "Interfaces/Interface.hpp"
//system
#include "Systems/Ai/System.hpp"


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
InitAISystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateAISystem()
{
    return new AISystem();
}


void STDCALL
DestroyAISystem( ISystem* pSystem)
{
    delete reinterpret_cast<AISystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemAi = {
        &InitAISystem,
        &CreateAISystem,
        &DestroyAISystem
    };
}
