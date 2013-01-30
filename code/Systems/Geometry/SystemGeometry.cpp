//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/Geometry/System.hpp"


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
InitGeometrySystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateGeometrySystem()
{
    return new GeometrySystem();
}


void STDCALL
DestroyGeometrySystem( ISystem* pSystem)
{
    delete reinterpret_cast<GeometrySystem*>( pSystem );
}

extern "C" 
{
    struct SystemFuncs SystemGeometry = {
        &InitGeometrySystem,
        &CreateGeometrySystem,
        &DestroyGeometrySystem
    };
}
