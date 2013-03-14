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
#include <map>
#include <memory>
#include <mutex>

////////////////////////////////////////////////////////////////////////
/// Provides a single location for access to the different 
/// systems that are available for use.
////////////////////////////////////////////////////////////////////////

class SystemManager
{
private:
    // Singleton
    static std::shared_ptr<SystemManager> instance_;
    static std::once_flag                   only_one;
     
    SystemManager(const SystemManager& rs) {
        instance_  = rs.instance_;
    }
 
    SystemManager& operator = (const SystemManager& rs) 
    {
        if (this != &rs) {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    SystemManager() {}
    
public:

    static SystemManager& getInstance() 
    {
        std::call_once( SystemManager::only_one, [] () 
        { 
            SystemManager::instance_.reset( 
                new SystemManager()); 
        });
 
        return *SystemManager::instance_;
    }
    
    ~SystemManager() {}

    // Adds a new system to the collection.  
    // Called by the ISystem constructor.
    Error Add( ISystem* pSystem );

    // Removes a system from the collection.  
    // Called by the ISystem destructor.
    Error Remove( const System::Type SystemType );

    // Gets a specific system from the collection based on its type.
    ISystem* Get( const System::Type SystemType );

    // Gets a specific system from the collection base on its name.
    ISystem* Get( pcstr pszName );

    /* TODO: These need to be defined better. Maybe use something like 
     * the stl iterator. The owner of the Scheduler class needs to take 
     * care of this since it will be the primary consumer.*/
    ISystem* GetFirst( void );
    ISystem* GetNext( void );


protected:

    std::map<System::Type, ISystem*>              m_Systems;
    std::map<System::Type, ISystem*>::iterator    m_SystemIt;
};
