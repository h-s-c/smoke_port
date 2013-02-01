//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//stdlib
#include <iostream>
//framework
#include "Framework/SystemManager.hpp"


std::once_flag                   
SystemManager::only_one;
std::shared_ptr<SystemManager> 
SystemManager::instance_ = nullptr;

typedef std::map<Id, ISystem*>::iterator SystemMapIter;
typedef std::map<Id, ISystem*>::const_iterator SystemMapConstIter;

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
