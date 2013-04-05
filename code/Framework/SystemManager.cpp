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
#include "Framework/PlatformManager.hpp"
#include "Framework/SystemManager.hpp"
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/TaskManager.hpp"

#include <iostream>
#include <dlfcn.h>

std::once_flag                   
SystemManager::only_one;
std::shared_ptr<SystemManager> 
SystemManager::instance_ = nullptr;

typedef std::map<Id, ISystem*>::iterator SystemMapIter;
typedef std::map<Id, ISystem*>::const_iterator SystemMapConstIter;

struct ManagerInterfaces
{
	IEnvironment*       pEnvironment;
	IService*           pService;
	ITaskManager*       pTask;
	IPlatform*          pPlatform;
};

struct SystemFuncs 
{
	std::function<void STDCALL (ManagerInterfaces* pManagers)> InitSystem;
	std::function<ISystem* STDCALL ()> CreateSystem;
	std::function<void STDCALL (ISystem* pSystem)> DestroySystem;
};

SystemManager::~SystemManager()
{
    // Iterate through all the loaded libraries.
    for ( auto it : m_SystemLibs)
    {
        struct SystemFuncs *pSystemFuncs = reinterpret_cast<SystemFuncs*>(dlsym(it.hLib, it.strSysLib.c_str() ));
        // System creation/destruction need to happen on the same thread
        pSystemFuncs->DestroySystem(it.pSystem);
        dlclose( it.hLib );
    }

    m_SystemLibs.clear();
}

Error
SystemManager::LoadSystemLibrary(
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
            Managers.pTask        = &TaskManager::getInstance();
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



Error
SystemManager::Add(
    ISystem* pSystem
    )
{
    System::Type SystemType = pSystem->GetSystemType();    
    if ( m_Systems.find( SystemType ) != m_Systems.end() )
    {
        std::cerr << "m_Systems.find( SystemType ) != m_Systems.end()" << std::endl;
    }
    
    m_Systems[ SystemType ] = pSystem;

    return Errors::Success;
}


Error
SystemManager::Remove(
    const System::Type SystemType
    )
{
    Error   Err = Errors::Success;

    SystemMapIter it = m_Systems.find( SystemType );

    if ( it != m_Systems.end() )
    {
        m_Systems.erase( it );
    }

    return Err;
}


ISystem*
SystemManager::Get(
    const System::Type SystemType
    )
{
    ISystem* pSystem = NULL;

    SystemMapConstIter it = m_Systems.find( SystemType );

    if ( it != m_Systems.end() )
    {
        pSystem = (*it).second;
    }


    return pSystem;
}


ISystem*
SystemManager::Get(
    pcstr pszName
    )
{
    ISystem* pSystem = NULL;

    for ( SystemMapConstIter it = m_Systems.begin();
          it != m_Systems.end(); it++ )
    {
        if ( strcmp( pszName, it->second->GetName() ) == 0 )
        {
            pSystem = it->second;
            break;
        }
    }

    return pSystem;
}


ISystem*
SystemManager::GetFirst(
    void
    )
{
    ISystem* pSystem = NULL;

    m_SystemIt = m_Systems.begin();

    if ( m_SystemIt != m_Systems.end() )
    {
        pSystem = (*m_SystemIt).second;
    }

    return pSystem;
}


ISystem*
SystemManager::GetNext(
    void
    )
{
    ISystem* pSystem = NULL;

    if ( ++m_SystemIt != m_Systems.end() )
    {
        pSystem = (*m_SystemIt).second;
    }

    return pSystem;
}
