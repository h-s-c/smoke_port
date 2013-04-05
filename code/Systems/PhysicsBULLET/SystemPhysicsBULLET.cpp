#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Common/System.hpp"
#include "Systems/PhysicsBULLET/System.hpp"

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
InitBulletPhysicsSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateBulletPhysicsSystem()
{
    return new BulletPhysicsSystem();
}


void STDCALL
DestroyBulletPhysicsSystem( ISystem* pSystem)
{
    delete reinterpret_cast<BulletPhysicsSystem*>( pSystem );
}

extern "C" 
{
    DLLEXPORT struct SystemFuncs SystemPhysicsBULLET = {
        &InitBulletPhysicsSystem,
        &CreateBulletPhysicsSystem,
        &DestroyBulletPhysicsSystem
    };
}
