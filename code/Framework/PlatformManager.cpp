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

// Prevent other headers from including any platform specific ones
#define __PLATFORM_MANAGER
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"

#include "Framework/PlatformManager.hpp"
#include "Framework/SystemManager.hpp"
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/TaskManager.hpp"

std::once_flag                   
PlatformManager::only_one;
std::shared_ptr<PlatformManager> 
PlatformManager::instance_ = nullptr;

extern TaskManager*     g_pTaskManager;

#if defined (PLATFORM_WINDOWS)
namespace Windows
{
    #define _WIN32_WINNT    0x0400 
    #include <windows.h>
}


PlatformManager::FileSystem::~FileSystem(
    void
    )
{
    //
    // Iterate through all the loaded libraries.
    //
    std::vector<SystemLib>::const_iterator it;
    for ( it=m_SystemLibs.begin(); it!=m_SystemLibs.end(); it++ )
    {
        Windows::HMODULE hLib = reinterpret_cast<Windows::HMODULE>(it->hLib);

        //
        // Get the system destruction function.
        //
        DestroySystemFunction fnDestroySystem =
            reinterpret_cast<DestroySystemFunction>(
                Windows::GetProcAddress( hLib, "DestroySystem" )
                );

        if ( fnDestroySystem != NULL )
        {
            fnDestroySystem( it->pSystem );
        }

        Windows::FreeLibrary( hLib );
    }

    m_SystemLibs.clear();
}


Error
PlatformManager::FileSystem::LoadSystemLibrary(
    const char* pszSysLib,
    ISystem** ppSystem
    )
{
    Error   Err = Errors::Failure;

    //
    // Load the dll.
    //
    Windows::HMODULE hLib = Windows::LoadLibraryA( pszSysLib );

    if ( hLib != NULL )
    {
        //
        // Get the system initialization function.
        //
        InitializeSystemLibFunction fnInitSystemLib =
            reinterpret_cast<InitializeSystemLibFunction>(
                Windows::GetProcAddress( hLib, "InitializeSystemLib" )
                );

        if ( fnInitSystemLib != NULL )
        {
            ManagerInterfaces Managers;
            Managers.pEnvironment = &Singletons::EnvironmentManager;
            Managers.pService     = &Singletons::ServiceManager;
            Managers.pTask        = g_pTaskManager;

            fnInitSystemLib( &Managers );
        }

        //
        // Get the system creation function.
        //
        CreateSystemFunction fnCreateSystem =
            reinterpret_cast<CreateSystemFunction>(
                Windows::GetProcAddress( hLib, "CreateSystem" )
                );

        if ( fnCreateSystem != NULL )
        {
            //
            // Create the system.
            //
            ISystem* pSystem = fnCreateSystem( );

            if ( pSystem != NULL )
            {
                //
                // Verify that there's no duplicate system type.
                //
                System::Type SystemType = pSystem->GetSystemType();

                ISystem* pCurrSystem =
                    Singletons::SystemManager.Get( SystemType );

                if ( pCurrSystem == NULL )
                {
                    //
                    // Add the system to the collection.
                    //
                    Singletons::SystemManager.Add( pSystem );

                    SystemLib sl = { reinterpret_cast<Handle>(hLib), pSystem };
                    m_SystemLibs.push_back( sl );

                    *ppSystem = pSystem;
                }
            }
        }
    }

    return Err;
}

void
PlatformManager::WindowSystem::ProcessMessages(
    void
    )
{
    //
    // Process all messages in the queue.
    //
    Windows::MSG    Msg;

    while ( Windows::PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
    {
        Windows::TranslateMessage( &Msg );
        Windows::DispatchMessage( &Msg );
    }
}

#elif defined (PLATFORM_UNIX)

#include <dlfcn.h>

PlatformManager::FileSystem::~FileSystem(
    void
    )
{
    //
    // Iterate through all the loaded libraries.
    //
    std::vector<SystemLib>::const_iterator it;
    for ( it=m_SystemLibs.begin(); it!=m_SystemLibs.end(); it++ )
    {
        void* hLib = reinterpret_cast<void*>(it->hLib);

        //
        // Get the system destruction function.
        //
        DestroySystemFunction fnDestroySystem =
            reinterpret_cast<DestroySystemFunction>(
                dlsym( hLib, "DestroySystem" )
                );

        if ( fnDestroySystem != NULL )
        {
            fnDestroySystem( it->pSystem );
        }

        dlclose( hLib );
    }

    m_SystemLibs.clear();
}


Error
PlatformManager::FileSystem::LoadSystemLibrary(
    const char* pszSysLib,
    ISystem** ppSystem
    )
{
    Error   Err = Errors::Failure;

    //
    // Load the dll.
    //
    void* hLib = dlopen( pszSysLib, RTLD_NOW);

    if ( hLib != NULL )
    {
        //
        // Get the system initialization function.
        //
        InitializeSystemLibFunction fnInitSystemLib =
            reinterpret_cast<InitializeSystemLibFunction>(
                dlsym( hLib, "InitializeSystemLib" )
                );

        if ( fnInitSystemLib != NULL )
        {
            ManagerInterfaces Managers;
            Managers.pEnvironment = &EnvironmentManager::getInstance();
            Managers.pService     = &ServiceManager::getInstance();
            Managers.pTask        = g_pTaskManager;

            fnInitSystemLib( &Managers );
        }

        //
        // Get the system creation function.
        //
        CreateSystemFunction fnCreateSystem =
            reinterpret_cast<CreateSystemFunction>(
                dlsym( hLib, "CreateSystem" )
                );

        if ( fnCreateSystem != NULL )
        {
            //
            // Create the system.
            //
            ISystem* pSystem = fnCreateSystem( );

            if ( pSystem != NULL )
            {
                //
                // Verify that there's no duplicate system type.
                //
                System::Type SystemType = pSystem->GetSystemType();

                ISystem* pCurrSystem =
                    SystemManager::getInstance().Get( SystemType );

                if ( pCurrSystem == NULL )
                {
                    //
                    // Add the system to the collection.
                    //
                    SystemManager::getInstance().Add( pSystem );

                    SystemLib sl = { reinterpret_cast<Handle>(hLib), pSystem };
                    m_SystemLibs.push_back( sl );

                    *ppSystem = pSystem;
                }
            }
        }
    }

    return Err;
}

void
PlatformManager::WindowSystem::ProcessMessages(
    void
    )
{
    //
    // Process all messages in the queue.
    //
}

#endif
