//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/System.hpp"


ManagerInterfaces   g_Managers;

#if defined(COMPILER_MSVC)
#include <windows.h>

void*               g_hInstance;


BOOL APIENTRY
DllMain(
    HMODULE hModule,
    DWORD Reason,
    LPVOID pReserved
    )
{
    UNREFERENCED_PARAM( pReserved );

    switch ( Reason )
    {
    case DLL_PROCESS_ATTACH:
        g_hInstance = hModule;
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
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
CreateSystem( )
{
    return new OGREGraphicsSystem();
}


extern "C" void STDCALL
DestroySystem(
    ISystem* pSystem
    )
{
    delete reinterpret_cast<OGREGraphicsSystem*>( pSystem );
}

