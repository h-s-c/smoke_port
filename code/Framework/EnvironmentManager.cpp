//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//stdlib
#include <iostream>
//framework
#include "Framework/EnvironmentManager.hpp"

std::once_flag             
EnvironmentManager::only_one;
std::shared_ptr<EnvironmentManager> 
EnvironmentManager::instance_ = nullptr;

void
EnvironmentManager::Variables::Add(
    In pcstr pszName,
    In pcstr pszValue
    )
{
    Variables::Pair vp;
    vp.sName = pszName;
    vp.sValue = pszValue;

    m_Pairs.push_back( vp );
}

// Set the value of an environment variable that already exists.  
// Environment variables are defined in Smoke.gdf.
void
EnvironmentManager::Variables::SetValue(
    In pcstr pszName,
    In pcstr pszValue
    )
{
    for ( PairsIt it=m_Pairs.begin(); it != m_Pairs.end(); it++ )
    {
        Pair& vp = *it;

        if ( vp.sName == pszName )
        {
            vp.sValue = pszValue;
            break;
        }
    }

}


pcstr
EnvironmentManager::Variables::GetAsString(
    In pcstr pszName,
    In pcstr pszDefaultValue
    )
{
    pcstr pszValue = pszDefaultValue;

    GetValue( pszName, pszValue );

    return pszValue;
}


Bool
EnvironmentManager::Variables::IsString(
    In pcstr pszName,
    In pcstr pszCompareValue
    )
{
    Bool bValue = False;
    pcstr pszValue = NULL;

    GetValue( pszName, pszValue );
    if( pszValue != NULL )
    {
        bValue = ( _stricmp( pszValue, pszCompareValue ) == 0 );
    }

    return bValue;
}


Bool
EnvironmentManager::Variables::GetAsBool(
    In pcstr pszName,
    In Bool bDefaultValue
    )
{
    Bool bValue = bDefaultValue;

    pcstr pszValue;
    if ( GetValue( pszName, pszValue ) )
    {
        if(!_stricmp( pszValue, "False" ) && !_stricmp( pszValue, "True" ))
        {
            std::cerr << "Bool '" << pszName << "' not True or False. It is " << pszValue << "." << std::endl;
        }
        bValue = _stricmp( pszValue, "True" ) == 0;
    }

    return bValue;
}


i32
EnvironmentManager::Variables::GetAsInt(
    In pcstr pszName,
    In i32 DefaultValue
    )
{
    i32 Value = DefaultValue;

    pcstr pszValue;
    if ( GetValue( pszName, pszValue ) )
    {
        Value = atoi( pszValue );
    }

    return Value;
}


f32
EnvironmentManager::Variables::GetAsFloat(
    In pcstr pszName,
    In f32 DefaultValue
    )
{
    f32 Value = DefaultValue;

    pcstr pszValue;
    if ( GetValue( pszName, pszValue ) )
    {
        Value = static_cast<f32>(atof( pszValue ));
    }

    return Value;
}


Bool
EnvironmentManager::Variables::GetValue(
    In pcstr pszName,
    Out pcstr& pszValue
    )
{
    Bool bFound = False;

    for ( PairsIt it=m_Pairs.begin(); it != m_Pairs.end(); it++ )
    {
        const Pair& vp = *it;

        if ( vp.sName == pszName )
        {
            pszValue = vp.sValue.c_str();
            bFound = True;
            break;
        }
    }

    return bFound;
}


IEnvironment::IRuntime::Status
EnvironmentManager::GetStatus(
    void
    )
{
    return m_RuntimeStatus;
}


void
EnvironmentManager::SetStatus(
    IEnvironment::IRuntime::Status Status
    )
{
    m_RuntimeStatus.exchange(Status);
}
