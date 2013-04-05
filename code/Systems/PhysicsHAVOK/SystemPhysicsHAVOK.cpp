//Writen by Hugh Smith summer and fall 2007
// Part of Smoke Framework implementation for procedural Fire system.

// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Common/System.hpp"
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
