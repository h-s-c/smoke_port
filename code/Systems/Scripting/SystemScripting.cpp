// Copyright © 2008 Intel Corporation
// All Rights Reserved
// 
// The sample source code contained or described herein and all documents
// related to the sample source code ("Material") are owned by Intel Corporation
// or its suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material may contain trade secrets and
// proprietary and confidential information of Intel Corporation and its suppliers
// and licensors, and is protected by worldwide copyright and trade secret laws and
// treaty provisions. The sample source code is provided AS IS, with no warranties
// of any kind, express or implied. Except as expressly permitted by the Software
// license, neither Intel Corporation nor its suppliers assumes any responsibility
// or liability for any errors or inaccuracies that may appear herein.

#pragma unmanaged

#include <windows.h>

//
// core includes
//
#pragma warning( disable : 4633 )
#include "..\BaseTypes\BaseTypes.h"
#include "..\Interfaces\Interface.h"

//
// system includes
//
#include "System.h"


ManagerInterfaces   g_Managers;


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


extern "C" void __stdcall
InitializeScripting(
    ManagerInterfaces* pManagers
    )
{
    g_Managers = *pManagers;
}


extern "C" ISystem* __stdcall
CreateScriptingSystem(
    Debug::Debugger* p_Debugger
    )
{
	Debug::Init( p_Debugger );
    return new ScriptingSystem();
}


extern "C" void __stdcall
DestroyScriptingSystem(
    ISystem* pSystem
    )
{
	delete reinterpret_cast<ScriptingSystem*>( pSystem );
}
