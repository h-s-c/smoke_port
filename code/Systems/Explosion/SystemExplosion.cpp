//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Explosion/System.hpp"

#if defined(COMPILER_MSVC)
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
// DllMain - API entry point for SystemExplosion DLL
BOOL APIENTRY DllMain( HMODULE hModule, DWORD Reason, LPVOID pReserved )
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

ManagerInterfaces   g_Managers;

///////////////////////////////////////////////////////////////////////////////
// InitializeNewtonPhysicsCollision - Initialize the Explosion system
extern "C" void STDCALL InitializeSystemLib( ManagerInterfaces* pManagers )
{
    g_Managers = *pManagers;
}



///////////////////////////////////////////////////////////////////////////////
// CreateAISystem - Create the Explosion system
extern "C" ISystem* STDCALL CreateSystem()
{
    // Create the Explosion system
    return new ExplosionSystem();
}


///////////////////////////////////////////////////////////////////////////////
// DestroyAISystem - Release all resources allocated for the given SystemAI
extern "C" void STDCALL DestroySystem( ISystem* pSystem )
{
    ExplosionSystem* pExplosionSystem = reinterpret_cast<ExplosionSystem*>(pSystem);
    SAFE_DELETE( pExplosionSystem );
}

