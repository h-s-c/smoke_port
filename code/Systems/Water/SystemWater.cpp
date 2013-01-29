// Copyright � 2008-2009 Intel Corporation
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


#include <windows.h>

//
// core includes
//
#include "..\BaseTypes\BaseTypes.h"
#include "..\Interfaces\Interface.h"

//
// system includes
//
#include "System.h"

ManagerInterfaces   g_Managers;

///////////////////////////////////////////////////////////////////////////////
// DllMain - API entry point for SystemWater DLL
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


///////////////////////////////////////////////////////////////////////////////
// InitializeNewtonPhysicsCollision - Initialize the Water system
extern "C" void __stdcall InitializeWaterSystem( ManagerInterfaces* pManagers )
{
    g_Managers = *pManagers;
}



///////////////////////////////////////////////////////////////////////////////
// CreateWaterSystem - Create the Water system
extern "C" ISystem* __stdcall CreateWaterSystem( Debug::Debugger* p_Debugger )
{
	// Set up the debugger
	Debug::Init( p_Debugger );

	// Create the Water system
    return new WaterSystem();
}


///////////////////////////////////////////////////////////////////////////////
// DestroyWaterSystem - Release all resources allocated for the given SystemWater
extern "C" void __stdcall DestroyWaterSystem( ISystem* pSystem )
{
    WaterSystem* pWaterSystem = reinterpret_cast<WaterSystem*>(pSystem);
    SAFE_DELETE( pWaterSystem );
}

