//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/System.hpp"


ManagerInterfaces   g_Managers;

#if defined(COMPILER_MSVC)
#include <windows.h>

BOOL APIENTRY
DllMain(
    HMODULE hModule,
    DWORD Reason,
    LPVOID pReserved
    )
{
    UNREFERENCED_PARAM( hModule );
    UNREFERENCED_PARAM( pReserved );

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

extern "C" void STDCALL
InitializeSystemLib( 
    ManagerInterfaces* pManagers 
    )
{
    g_Managers = *pManagers;
}

extern "C" ISystem* STDCALL
CreateSystem()
{
    return new FMODSystem();
}


extern "C" void STDCALL
DestroySystem(
    ISystem* pSystem
    )
{
    delete reinterpret_cast<FMODSystem*>( pSystem );
}

