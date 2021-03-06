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
#include "Base/Library.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Framework/PlatformManager.hpp"
#include "Framework/SystemManager.hpp"
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/TaskManager.hpp"

#include <iostream>


std::once_flag                   
SystemManager::only_one;
std::shared_ptr<SystemManager> 
SystemManager::instance_ = nullptr;

typedef std::map<Id, ISystem*>::iterator SystemMapIter;
typedef std::map<Id, ISystem*>::const_iterator SystemMapConstIter;

SystemManager::~SystemManager()
{
    // Iterate through all the loaded libraries.
    for ( auto it : m_SystemLibs)
    {
        struct SystemFuncs *pSystemFuncs = reinterpret_cast<SystemFuncs*>(Base::GetSymbol(it.hLib, it.strSysLib.c_str() ));
        // System creation/destruction need to happen on the same thread
        pSystemFuncs->DestroySystem(it.pSystem);
        Base::CloseLibrary( it.hLib );
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
    
    void* hLib = Base::OpenLibrary(strSysLib, strSysLibPath);
    // Get the system functions struct.
    struct SystemFuncs *pSystemFuncs = reinterpret_cast<SystemFuncs*>(Base::GetSymbol(hLib, strSysLib.c_str() ));
    if (pSystemFuncs)
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

        if (pSystem)
        {
            // Verify that there's no duplicate system type.
            System::Type SystemType = pSystem->GetSystemType();

            ISystem* pCurrSystem = this->Get( SystemType );

            if (!pCurrSystem)
            {
                // Add the system to the collection.
                this->Add( pSystem );

                SystemLib sl = { hLib, pSystem, strSysLib};
                m_SystemLibs.push_back( sl );

                *ppSystem = pSystem;
            }
        }
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

    auto it = m_Systems.find( SystemType );

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
    ISystem* pSystem = nullptr;

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
    ISystem* pSystem = nullptr;

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
    ISystem* pSystem = nullptr;

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
    ISystem* pSystem = nullptr;

    if ( ++m_SystemIt != m_Systems.end() )
    {
        pSystem = (*m_SystemIt).second;
    }

    return pSystem;
}
