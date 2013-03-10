// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Standard Library
#include <iostream>
#include <dlfcn.h>
// Framework
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

PlatformManager::SystemLibrary::~SystemLibrary( void)
{
    // Iterate through all the loaded libraries.
    for ( auto it : m_SystemLibs)
    {
        struct SystemFuncs *pSystemFuncs = reinterpret_cast<SystemFuncs*>(dlsym(it.hLib, it.strSysLib.c_str() ));
        pSystemFuncs->DestroySystem( it.pSystem );
        dlclose( it.hLib );
    }

    m_SystemLibs.clear();
}


Error
PlatformManager::SystemLibrary::LoadSystemLibrary(
    const std::string strSysLib,
    const std::string strSysLibPath,
    ISystem** ppSystem
    )
{
    Error   Err = Errors::Failure;
    
    #if defined(__GNUC__) || defined(__clang__)
    // Load the .so
    void* hLib = dlopen( std::string(strSysLibPath + "/" + strSysLib + ".so").c_str(), RTLD_NOW);
    #elif defined(_MSC_VER)
    // Load the .dll
    void* hLib = dlopen( std::string(strSysLibPath + "/" + strSysLib + ".dll").c_str(), RTLD_NOW);
    #endif
    
    if ( hLib != NULL )
    {
        // Get the system functions struct.
        struct SystemFuncs *pSystemFuncs = reinterpret_cast<SystemFuncs*>(dlsym(hLib, strSysLib.c_str() ));
        if ( pSystemFuncs != NULL )
        {
            ManagerInterfaces Managers;
            Managers.pEnvironment = &EnvironmentManager::getInstance();
            Managers.pService     = &ServiceManager::getInstance();
            Managers.pTask        = g_pTaskManager;
            Managers.pPlatform    = &PlatformManager::getInstance();

            // Initialize the system.
            pSystemFuncs->InitSystem( &Managers );

            // Create the system.
            ISystem* pSystem = pSystemFuncs->CreateSystem();

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

                    SystemLib sl = { hLib, pSystem, strSysLib};
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

size_t
PlatformManager::Window::GetWindowHandle( void )
{
    return this->window;
}

void 
PlatformManager::Window::SetWindowHandle(size_t window)
{
    this->window = window;
}

uint32_t
PlatformManager::Window::GetWindowHeight( void )
{
    return this->height;
}

void 
PlatformManager::Window::SetWindowHeight(uint32_t height)
{
    this->height = height;
}

uint32_t
PlatformManager::Window::GetWindowWidth( void )
{
    return this->width;
}

void 
PlatformManager::Window::SetWindowWidth(uint32_t width)
{
    this->width = width;
}
