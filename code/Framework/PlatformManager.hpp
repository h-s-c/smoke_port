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
#include <memory>
#include <mutex>
#include <vector>

class PlatformManager : public IPlatform
{
private:
    // Singleton
    static std::shared_ptr<PlatformManager> instance_;
    static std::once_flag                   only_one;
     
    PlatformManager(const PlatformManager& rs) {
        instance_  = rs.instance_;
    }
 
    PlatformManager& operator = (const PlatformManager& rs) 
    {
        if (this != &rs) {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    PlatformManager() {}
    
public:

    static PlatformManager& getInstance() 
    {
        std::call_once( PlatformManager::only_one, [] () 
        { 
            PlatformManager::instance_.reset( new PlatformManager()); 
        });
 
        return *PlatformManager::instance_;
    }
    
    ~PlatformManager() {}
    

    ////////////////////////////////////////////////////////////////////
    ///   Provides OS shared library loading functionality.
    ////////////////////////////////////////////////////////////////////
    class SystemLibrary
    {
    private:
        friend class PlatformManager;

    protected:
        // Only accessible via the PlatformMananger.
        SystemLibrary() {}
        ~SystemLibrary();
        
    public:
        // Loads a system library and returns pointers to the system.
        Error LoadSystemLibrary( const std::string strSysLib,
                                    const std::string strSysLibPath,
                                    ISystem** ppSystem);

    protected:
        struct SystemLib
        {
            void*                     hLib;
            ISystem*                  pSystem;
            const std::string         strSysLib;
        };
        std::vector<SystemLib>  m_SystemLibs;
    private:
    };


    ////////////////////////////////////////////////////////////////////
    ///   Provides OS window system functionality.
    ////////////////////////////////////////////////////////////////////
    class Window : public IPlatform::IWindow
    {
    private:
        friend class PlatformManager;

    protected:
        // Only accessible via the PlatformMananger.</remarks>
        Window() {}

    public:
        // Get window handle.
        virtual size_t GetWindowHandle( void );
        // Set window handle.
        virtual void SetWindowHandle(size_t window);
        // Get window height.
        virtual uint32_t GetWindowHeight(void);
        // Set window height.
        virtual void SetWindowHeight(uint32_t height);
        // Get window width.
        virtual uint32_t GetWindowWidth(void);
        // Set window width.
        virtual void SetWindowWidth(uint32_t width);
    private:
        size_t window;
        uint32_t height;
        uint32_t width;
    };

protected:
    SystemLibrary                   m_SystemLibrary;
    Window                          m_Window;

public:
    // Gets a reference to the SystemLibrary class.
     SystemLibrary& SystemLibrary( void )
    {
        return m_SystemLibrary;
    }

    // Gets a reference to the Window class.
    virtual Window& Window( void )
    {
        return m_Window;
    }
    
};
