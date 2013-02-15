//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/Explosion/System.hpp"


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
InitExplosionSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateExplosionSystem()
{
    return new ExplosionSystem();
}


void STDCALL
DestroyExplosionSystem( ISystem* pSystem)
{
    delete reinterpret_cast<ExplosionSystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemExplosion = {
        &InitExplosionSystem,
        &CreateExplosionSystem,
        &DestroyExplosionSystem
    };
}
