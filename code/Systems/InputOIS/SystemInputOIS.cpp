//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/InputOIS/System.hpp"


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
InitInputSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateInputSystem()
{
    return new InputSystem();
}


void STDCALL
DestroyInputSystem( ISystem* pSystem)
{
    delete reinterpret_cast<InputSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemInputOIS = {
        &InitInputSystem,
        &CreateInputSystem,
        &DestroyInputSystem
    };
}
