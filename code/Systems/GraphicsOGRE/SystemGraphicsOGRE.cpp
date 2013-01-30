//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/System.hpp"


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
InitOGREGraphicsSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateOGREGraphicsSystem()
{
    return new OGREGraphicsSystem();
}


void STDCALL
DestroyOGREGraphicsSystem( ISystem* pSystem)
{
    delete reinterpret_cast<OGREGraphicsSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemGraphicsOGRE = {
        &InitOGREGraphicsSystem,
        &CreateOGREGraphicsSystem,
        &DestroyOGREGraphicsSystem
    };
}
