#pragma once
#include <memory>
#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////
///   Abstraction class for OS specific functionality.
////////////////////////////////////////////////////////////////////////
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
    ///   Provides OS file system functionality.
    ////////////////////////////////////////////////////////////////////
    class FileSystem
    {
    private:
        friend class PlatformManager;

    protected:
        // Only accessible via the PlatformMananger.
        FileSystem() {}
        ~FileSystem();
        
    public:
        // Loads a system library and returns pointers to the system.
        Error LoadSystemLibrary( In char* pszSysLib, 
                                 ISystem** ppSystem);

    protected:
        struct SystemLib
        {
            void*               hLib;
            ISystem*            pSystem;
        };
        std::vector<SystemLib>  m_SystemLibs;
    };


    ////////////////////////////////////////////////////////////////////
    ///   Provides OS window system functionality.
    ////////////////////////////////////////////////////////////////////
    class WindowSystem
    {
    private:
        friend class PlatformManager;

    protected:
        // Only accessible via the PlatformMananger.</remarks>
        WindowSystem() {}

    public:
        // Processes any waiting window messages.
        void ProcessMessages( void );
    };

protected:
    FileSystem                          m_FileSystem;
    WindowSystem                        m_WindowSystem;

public:
    // Gets a reference to the FileSystem class.
    FileSystem& FileSystem( void )
    {
        return m_FileSystem;
    }

    // Gets a reference to the WindowSystem class.
    WindowSystem& WindowSystem( void )
    {
        return m_WindowSystem;
    }
};
