#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/PhysicsHAVOK/System.hpp"



#if defined(COMPILER_MSVC)
#include <windows.h>

//http://software.intel.com/en-us/forums/topic/290061
#include <Common/Base/Types/hkBaseTypes.h>
struct hkTestEntry* hkUnitTestDatabase = HK_NULL;
hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line)
{
    return false;
}



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
InitHavokPhysicsSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}


ISystem* STDCALL
CreateHavokPhysicsSystem()
{
    return new HavokPhysicsSystem();
}


void STDCALL
DestroyHavokPhysicsSystem( ISystem* pSystem)
{
    delete reinterpret_cast<HavokPhysicsSystem*>( pSystem );
}

extern "C" 
{
    DLLEXPORT struct SystemFuncs SystemPhysicsHAVOK = {
        &InitHavokPhysicsSystem,
        &CreateHavokPhysicsSystem,
        &DestroyHavokPhysicsSystem
    };
}
