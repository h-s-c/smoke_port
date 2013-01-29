#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"

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
    ASSERT( m_pSystemAccess != NULL );
    return *m_pSystemAccess;
}


void
ServiceManager::RegisterSystemAccessProvider(
    ISystemAccess* pSystemAccess
    )
{
    ASSERTMSG( m_pSystemAccess == NULL, 
               "There is already an existing system access provider." );
    m_pSystemAccess = pSystemAccess;
}


void
ServiceManager::UnregisterSystemAccessProvider(
    ISystemAccess* pSystemAccess
    )
{
    ASSERTMSG( m_pSystemAccess == pSystemAccess,
               "You are not the registered system access provider." );

    if ( m_pSystemAccess == pSystemAccess )
    {
        m_pSystemAccess = NULL;
    }
}


IService::ICollision&
ServiceManager::Collision(
    void
    )
{
    ASSERT( m_pCollision != NULL );
    return *m_pCollision;
}


void
ServiceManager::RegisterCollisionProvider(
    ICollision* pCollision
    )
{
    ASSERTMSG( m_pCollision == NULL, 
               "There is already an existing collision provider." );
    m_pCollision = pCollision;
}


void
ServiceManager::UnregisterCollisionProvider(
    ICollision* pCollision
    )
{
    ASSERTMSG( m_pCollision == pCollision, 
               "You are not the registered collision provider." );

    if ( m_pCollision == pCollision )
    {
        m_pCollision = NULL;
    }
}
