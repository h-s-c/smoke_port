// base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// interface
#include "Interfaces/Interface.hpp"
// stdlib
#include <iostream>
// framework
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
    // Iterate through all the loaded libraries.
    std::vector<SystemLib>::const_iterator it;
    for ( it=m_SystemLibs.begin(); it!=m_SystemLibs.end(); it++ )
    {
        Windows::HMODULE hLib = reinterpret_cast<Windows::HMODULE>(it->hLib);
        
        // Get the system functions struct.
        struct SystemFuncs *systemFuncs = reinterpret_cast<SystemFuncs*>(Windows::GetProcAddress(hLib, it->pSystem->GetName()));
        if ( systemFuncs != NULL )
        {
            systemFuncs->DestroySystem( it->pSystem );
        }
        else
        {
            std::cerr << "Could not get the system functions from " << it->pSystem->GetName() << std::endl;
        }
        
        Windows::FreeLibrary( hLib );
    }

    m_SystemLibs.clear();
}


Error
PlatformManager::FileSystem::LoadSystemLibrary(
    const std::string strSysLib,
    const std::string strSysLibPath,
    ISystem** ppSystem
    )
{
    Error   Err = Errors::Failure;
    
    // Load the dll.
    Windows::HMODULE hLib = Windows::LoadLibraryA( std::string(strSysLibPath + "/" + strSysLib + ".dll").c_str() );

    if ( hLib != NULL )
    {
        // Get the system functions struct.
        struct SystemFuncs *systemFuncs = reinterpret_cast<SystemFuncs*>(Windows::GetProcAddress(hLib, strSysLib.c_str()));
        if ( systemFuncs != NULL )
        {
            ManagerInterfaces Managers;
            Managers.pEnvironment = &EnvironmentManager::getInstance();
            Managers.pService     = &ServiceManager::getInstance();
            Managers.pTask        = g_pTaskManager;
            Managers.pPlatform    = &PlatformManager::getInstance();

            // Initialize the system.
            systemFuncs->InitSystem( &Managers );

            // Create the system.
            ISystem* pSystem = systemFuncs->CreateSystem( );

            if ( pSystem != NULL )
            {
                // Verify that there's no duplicate system type.
                System::Type SystemType = pSystem->GetSystemType();

                ISystem* pCurrSystem =
                    SystemManager::getInstance().Get( SystemType );

                if ( pCurrSystem == NULL )
                {
                    // Add the system to the collection.
                    SystemManager::getInstance().Add( pSystem );

                    SystemLib sl = { reinterpret_cast<Handle>(hLib), pSystem };
                    m_SystemLibs.push_back( sl );

                    *ppSystem = pSystem;
                }
            }
        }
        else
        {
            std::cerr << "Could not get the system functions from " << strSysLib.c_str() << std::endl;
        }
    }
    else
    {
        std::cerr << "Could not open " << strSysLib.c_str() << std::endl;
    }

    return Err;
}

void
PlatformManager::WindowSystem::ProcessMessages( void)
{
    // Process all messages in the queue.
    Windows::MSG    Msg;

    while ( Windows::PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
    {
        Windows::TranslateMessage( &Msg );
        Windows::DispatchMessage( &Msg );
    }
}

#elif defined (PLATFORM_UNIX)

// external
#include <dlfcn.h>
#include <boost/filesystem.hpp>

PlatformManager::FileSystem::~FileSystem( void)
{
    // Iterate through all the loaded libraries.
    std::vector<SystemLib>::const_iterator it;
    for ( it=m_SystemLibs.begin(); it!=m_SystemLibs.end(); it++ )
    {
        void* hLib = reinterpret_cast<void*>(it->hLib);
        
        // Get the system functions struct.
        struct SystemFuncs *systemFuncs = reinterpret_cast<SystemFuncs*>(dlsym(hLib, it->pSystem->GetName()));
        if ( systemFuncs != NULL )
        {
            systemFuncs->DestroySystem( it->pSystem );
        }
        else
        {
            std::cerr << dlerror() << std::endl;
        }

        dlclose( hLib );
    }

    m_SystemLibs.clear();
}


Error
PlatformManager::FileSystem::LoadSystemLibrary(
    const std::string strSysLib,
    const std::string strSysLibPath,
    ISystem** ppSystem
    )
{
    Error   Err = Errors::Failure;
    
    // Load the .so
    void* hLib = dlopen( std::string(strSysLibPath + "/" + strSysLib + ".so").c_str(), RTLD_NOW);

    if ( hLib != NULL )
    {
        // Get the system functions struct.
        struct SystemFuncs *systemFuncs = reinterpret_cast<SystemFuncs*>(dlsym(hLib, strSysLib.c_str() ));
        if ( systemFuncs != NULL )
        {
            ManagerInterfaces Managers;
            Managers.pEnvironment = &EnvironmentManager::getInstance();
            Managers.pService     = &ServiceManager::getInstance();
            Managers.pTask        = g_pTaskManager;
            Managers.pPlatform    = &PlatformManager::getInstance();

            // Initialize the system.
            systemFuncs->InitSystem( &Managers );

            // Create the system.
            ISystem* pSystem = systemFuncs->CreateSystem();

            if ( pSystem != NULL )
            {
                // Verify that there's no duplicate system type.
                System::Type SystemType = pSystem->GetSystemType();

                ISystem* pCurrSystem =
                    SystemManager::getInstance().Get( SystemType );

                if ( pCurrSystem == NULL )
                {
                    // Add the system to the collection.
                    SystemManager::getInstance().Add( pSystem );

                    SystemLib sl = { reinterpret_cast<Handle>(hLib), pSystem };
                    m_SystemLibs.push_back( sl );

                    *ppSystem = pSystem;
                }
            }
        }
        else
        {
            std::cerr << dlerror() << std::endl;
        }
    }
    else
    {
        std::cerr << dlerror() << std::endl;
    }

    return Err;
}

void
PlatformManager::WindowSystem::ProcessMessages(
    void
    )
{
    // Process all messages in the queue.
}

#endif

size_t
PlatformManager::WindowSystem::GetWindowHandle( void )
{
    return this->window;
}

void 
PlatformManager::WindowSystem::SetWindowHandle(size_t window)
{
    this->window = window;
}

uint32_t
PlatformManager::WindowSystem::GetWindowHeight( void )
{
    return this->height;
}

void 
PlatformManager::WindowSystem::SetWindowHeight(uint32_t height)
{
    this->height = height;
}

uint32_t
PlatformManager::WindowSystem::GetWindowWidth( void )
{
    return this->width;
}

void 
PlatformManager::WindowSystem::SetWindowWidth(uint32_t width)
{
    this->width = width;
}
