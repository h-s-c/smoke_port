//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/System.hpp"

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


///////////////////////////////////////////////////////////////////////////////
// InitializeNewtonPhysicsCollision - Initialize the AI system
extern "C" void STDCALL InitializeSystemLib( ManagerInterfaces* pManagers )
{
    g_Managers = *pManagers;
}



///////////////////////////////////////////////////////////////////////////////
// CreateAISystem - Create the AI system
extern "C" ISystem* STDCALL CreateSystem()
{
    // Create the AI system
    return new AISystem();
}


///////////////////////////////////////////////////////////////////////////////
// DestroyAISystem - Release all resources allocated for the given SystemAI
extern "C" void STDCALL DestroySystem( ISystem* pSystem )
{
    AISystem* pAISystem = reinterpret_cast<AISystem*>(pSystem);
    SAFE_DELETE( pAISystem );
}

