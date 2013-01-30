//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/AudioFMOD/System.hpp"


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
InitFMODAudioSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateFMODAudioSystem()
{
    return new FMODSystem();
}


void STDCALL
DestroyFMODAudioSystem( ISystem* pSystem)
{
    delete reinterpret_cast<FMODSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemAudioFMOD = {
        &InitFMODAudioSystem,
        &CreateFMODAudioSystem,
        &DestroyFMODAudioSystem
    };
}
