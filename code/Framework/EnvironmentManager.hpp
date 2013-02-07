#pragma once
#include <stddef.h> 
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////
///   Class for getting and setting information about the environment.
////////////////////////////////////////////////////////////////////////
class EnvironmentManager : public IEnvironment, 
                           public IEnvironment::IRuntime
{
private:
    // Singleton
    static std::shared_ptr<EnvironmentManager> instance_;
    static std::once_flag                   only_one;
     
    EnvironmentManager(const EnvironmentManager& rs) {
        instance_  = rs.instance_;
    }
 
    EnvironmentManager& operator = (const EnvironmentManager& rs) 
    {
        if (this != &rs) {
            instance_  = rs.instance_;
        }
 
        return *this;
    }

    EnvironmentManager() 
    : m_RuntimeStatus( IEnvironment::IRuntime::Status::Unknown ) {}
    
public:

    static EnvironmentManager& getInstance() 
    {
        std::call_once( EnvironmentManager::only_one, [] () 
        { 
            EnvironmentManager::instance_.reset( 
                new EnvironmentManager()); 
        });
 
        return *EnvironmentManager::instance_;
    }
    
    ~EnvironmentManager() {}

    ////////////////////////////////////////////////////////////////////
    ///   Environment variable functionality.
    ////////////////////////////////////////////////////////////////////

    class Variables : public IEnvironment::IVariables
    {
        friend class EnvironmentManager;

    public:

        // Adds a new environment variable.
        void Add( In pcstr pszName, In pcstr pszValue );

        // Sets new value for environment variable.
        void SetValue( In pcstr pszName, In pcstr pszValue );

        // Implementation of IEnvironment::IVariables::GetAsString.
        virtual pcstr GetAsString( In pcstr pszName, 
                                   In pcstr pszDefaultValue="" );

        // Implementation of IEnvironment::IVariables::IsString.
        virtual Bool IsString( In pcstr pszName, 
                               In pcstr pszCompareValue );

        // Implementation of IEnvironment::IVariables::GetAsBool.
        virtual Bool GetAsBool( In pcstr pszName, 
                                In Bool bDefaultValue=False );

        // Implementation of IEnvironment::IVariables::GetAsInt.
        virtual i32 GetAsInt( In pcstr pszName, 
                              In i32 DefaultValue=0 );

        // Implementation of IEnvironment::IVariables::GetAsFloat.
        virtual f32 GetAsFloat( In pcstr pszName, 
                                In f32 DefaultValue=0.0f );


    protected:

        // Only accessible via the EnvironmentManager class.
        Variables() {}

        // Gets the value of a variable.
        Bool GetValue( In pcstr pszName, Out pcstr& pszValue );


    protected:

        struct Pair
        {
            std::string                             sName;
            std::string                             sValue;
        };

        typedef std::vector<Pair>                   Pairs;
        typedef Pairs::iterator                     PairsIt;

        Pairs                                       m_Pairs;
    };


    ////////////////////////////////////////////////////////////////////
    ///   IEnvironment::IRuntime functionality.
    ////////////////////////////////////////////////////////////////////

    // Implementation of IEnvironment::IRuntime::GetStatus.
    virtual IEnvironment::IRuntime::Status GetStatus( void );

    // Implementation of IEnvironment::IRuntime::SetStatus.
    virtual void SetStatus( IEnvironment::IRuntime::Status Status );


protected:

    Variables                                       m_Variables;

    std::atomic<IEnvironment::IRuntime::Status>     m_RuntimeStatus;


public:

    // Implementation of IEnvironment::Variables.
    virtual Variables& Variables( void )
    {
        return m_Variables;
    }

    // Implementation of IEnvironment::Runtime.
    virtual IEnvironment::IRuntime& Runtime( void )
    {
        return *this;
    }
};
