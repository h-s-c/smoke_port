//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//stdlib
#include <iostream>
//framework
#include "Framework/SystemManager.hpp"
#include "Framework/ServiceManager.hpp"

std::once_flag                   
ServiceManager::only_one;
std::shared_ptr<ServiceManager> 
ServiceManager::instance_ = nullptr;

////////////////////////////////////////////////////////////////////////
// IService Implementations.

IService::ISystemAccess&
ServiceManager::SystemAccess(
    void
    )
{
    if ( m_pSystemAccess == NULL )
    {
        std::cerr << "m_pSystemAccess == NULL" << std::endl;
    }
    return *m_pSystemAccess;
}


void
ServiceManager::RegisterSystemAccessProvider(
    ISystemAccess* pSystemAccess
    )
{
    if ( m_pSystemAccess != NULL )
    {
        std::cerr << "There is already an existing system access provider." << std::endl;
    }
    m_pSystemAccess = pSystemAccess;
}


void
ServiceManager::UnregisterSystemAccessProvider(
    ISystemAccess* pSystemAccess
    )
{
    if ( m_pSystemAccess == pSystemAccess )
    {
        m_pSystemAccess = NULL;
    }
    else
    {
        std::cerr << "You are not the registered system access provider." << std::endl;
    }
}


IService::ICollision&
ServiceManager::Collision(
    void
    )
{
    if ( m_pCollision == NULL )
    {
        std::cerr << "m_pCollision == NULL." << std::endl;
    }
    
    return *m_pCollision;
}


void
ServiceManager::RegisterCollisionProvider(
    ICollision* pCollision
    )
{
    if ( m_pCollision != NULL )
    {
        std::cerr << "There is already an existing collision provider." << std::endl;
    }
    
    m_pCollision = pCollision;
}


void
ServiceManager::UnregisterCollisionProvider(
    ICollision* pCollision
    )
{
    if ( m_pCollision == pCollision )
    {
        m_pCollision = NULL;
    }
    else
    {
        std::cerr << "You are not the registered collision provider." << std::endl;
    }
}
