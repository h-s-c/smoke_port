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

#pragma once

////////////////////////////////////////////////////////////////////////
///   Class for providing services to the systems.
////////////////////////////////////////////////////////////////////////
class ServiceManager : public IService
{
private:
    // Singleton
    static std::shared_ptr<ServiceManager> instance_;
    static std::once_flag                   only_one;
     
    ServiceManager(const ServiceManager& rs) {
        instance_  = rs.instance_;
    }
 
    ServiceManager& operator = (const ServiceManager& rs) 
    {
        if (this != &rs) {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    ServiceManager() : m_pSystemAccess( NULL), m_pCollision( NULL) {}
    
public:

    static ServiceManager& getInstance() 
    {
        std::call_once( ServiceManager::only_one, [] () 
        { 
            ServiceManager::instance_.reset( 
                new ServiceManager()); 
        });
 
        return *ServiceManager::instance_;
    }
    
    ~ServiceManager() {}


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // IService Implementations.

    /// <summary cref="IService::SystemAccess">
    ///   Implementation of IService::SystemAccess.
    /// </summary>
    virtual IService::ISystemAccess& SystemAccess( void );

    /// <summary cref="IService::RegisterSystemAccessProvider">
    ///   Implementation of IService::RegisterSystemAccessProvider.
    /// </summary>
    virtual void RegisterSystemAccessProvider( ISystemAccess* pSystemAccess );

    /// <summary cref="IService::UnregisterSystemAccessProvider">
    ///   Implementation of IService::UnregisterSystemAccessProvider.
    /// </summary>
    virtual void UnregisterSystemAccessProvider( ISystemAccess* pSystemAccess );

    /// <summary cref="IService::Collision">
    ///   Implementation of IService::Collision.
    /// </summary>
    virtual IService::ICollision& Collision();

    /// <summary cref="IService::RegisterCollisionProvider">
    ///   Implementation of IService::RegisterCollisionProvider.
    /// </summary>
    virtual void RegisterCollisionProvider( ICollision* pCollision );

    /// <summary cref="IService::UnregisterCollisionProvider">
    ///   Implementation of IService::UnregisterCollisionProvider.
    /// </summary>
    virtual void UnregisterCollisionProvider( ICollision* pCollision );




protected:

    IService::ISystemAccess*            m_pSystemAccess;
    IService::ICollision*               m_pCollision;
};
