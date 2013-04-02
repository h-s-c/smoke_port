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

// base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// interface
#include "Interfaces/Interface.hpp"
// stdlib
#include <iostream>
#include <stdexcept>
#include <string>
// external
#if defined(_MSC_VER)
#include <filesystem>
#else
#include <boost/filesystem.hpp>
#endif
#include <tinyxml.h>
// framework
#include "Framework/Universal.hpp"
#include "Framework/SystemManager.hpp"
#include "Framework/PlatformManager.hpp"
#include "Framework/ChangeControlManager.hpp"
#include "Framework/EnvironmentManager.hpp"
#include "Framework/ServiceManager.hpp"
#include "Framework/Scheduler.hpp"
#include "Framework/TaskManager.hpp"
#include "Framework/Framework.hpp"


void
EngineExecuteGDF( pcstr pszGDF)
{
    Framework   Framework;

    pcstr GDF = (pszGDF != nullptr) ? pszGDF : "engine.gdf";
    if ( Framework.Initialize( GDF ) == Errors::Success )
    {
        Framework.Execute();
        Framework.Shutdown();
    }
}

Framework::Framework( void) 
    : m_pScheduler( nullptr )
    , m_pSceneCCM( nullptr )
    , m_pObjectCCM( nullptr )
    , m_bExecuteLoop( True )
{
    // m_pScheduler is instantiated after the environment variables
    // in the config file are parsed
    
    m_pObjectCCM = new ChangeManager();
    if (!m_pObjectCCM)
    {
        std::cerr << "m_pObjectCCM == NULL" << std::endl;
    }
    
    m_pSceneCCM = new ChangeManager();
    if (!m_pSceneCCM)
    {
        std::cerr << "m_pSceneCCM == NULL" << std::endl;
    }
}

Framework::~Framework( void)
{
    SAFE_DELETE( m_pScheduler );
    SAFE_DELETE( m_pSceneCCM );
    SAFE_DELETE( m_pObjectCCM );
}

Error
Framework::Initialize( pcstr pszGDF)
{
    std::clog << "Initializing Framework" << std::endl;
#if defined(_MSC_VER)
    // Backup directory
    auto oldpath = std::tr2::sys::current_path<std::tr2::sys::path>();
    // Go up one directory
    std::tr2::sys::current_path( std::tr2::sys::current_path<std::tr2::sys::path>().branch_path());
    // Check for GDF file
    if ( !std::tr2::sys::exists( std::tr2::sys::path(pszGDF)) )
#else
    // Backup directory
    auto oldpath = boost::filesystem::current_path();
    // Go up one directory
    boost::filesystem::current_path( boost::filesystem::current_path().parent_path());
    // Check for GDF file
    if ( !boost::filesystem::exists( boost::filesystem::path(pszGDF)))
#endif
    {
        std::cerr << "Framework could not locate the GDF file " << std::string(pszGDF) << "." << std::endl;
        return Errors::File::NotFound;
    }    
    m_sGDF = pszGDF;

    // Create the initial universal scene.
    m_pScene = new UScene( m_pSceneCCM, m_pObjectCCM );
    if ( m_pScene == nullptr )
    {
        std::cerr << "m_pScene == NULL" << std::endl;
        return Errors::Memory::OutOfMemory;
    }

    // Instantiate the parser, parse the environment variables in the GDF.
    GDFParser Parser( m_pScene, oldpath.string());
    Parser.ParseEnvironment( pszGDF );
    
    // Register the framework as the system access provider.  The system access provider gives the
    // ability for systems to set the properties in other systems.
    ServiceManager::getInstance().RegisterSystemAccessProvider( this );

    // Instantiate the scheduler.
    m_pScheduler = new Scheduler();
    if ( m_pScheduler == nullptr )
    {
        std::cerr << "m_pScheduler == NULL" << std::endl;
        return Errors::Memory::OutOfMemory;
    }

    // Complete the parsing of the GDF and the initial scene.
    m_sNextScene = Parser.Parse( pszGDF );
    m_sNextScene = Parser.ParseScene( pszGDF, m_sNextScene );

    // Set the initial scene for the scheduler.
    m_pScheduler->SetScene( m_pScene );

    return Errors::Success;
}

void
Framework::Shutdown( void)
{
    std::clog << "Shutting down Framework" << std::endl;

    // Get rid of the scene.
    SAFE_DELETE( m_pScene );

    // De-register the framework as the system access provider.
    ServiceManager::getInstance().UnregisterSystemAccessProvider( this );

    // Free resources used for parallel execution by the change manager.
    TaskManager::getInstance().PerThreadCallback( m_pObjectCCM->FreeThreadLocalData, m_pObjectCCM );    
    TaskManager::getInstance().PerThreadCallback( m_pSceneCCM->FreeThreadLocalData, m_pSceneCCM );  
}


Error
Framework::Execute( void)
{
    // Process the link messages in the CCMs first, for both the object and scene CCMs.  The link
    // needs to be established before any other messages come through.
    m_pObjectCCM->DistributeQueuedChanges(
        System::Types::All, System::Changes::Link | System::Changes::ParentLink
        );
    m_pSceneCCM->DistributeQueuedChanges(
        System::Types::All, System::Changes::Link | System::Changes::ParentLink
        );

    // Distribute changes for object and scene CCMs.  The UObject propagates some object messages
    // up to the scene so it needs to go first.
    m_pObjectCCM->DistributeQueuedChanges();
    m_pSceneCCM->DistributeQueuedChanges();

    // Set the runtime status to running.
    EnvironmentManager::getInstance().Runtime().SetStatus( IEnvironment::IRuntime::Status::Running );

    // Run through the main game loop.
    u32 StopAfterNFrames = EnvironmentManager::getInstance().Variables().GetAsInt( "StopAfterNFrames", 0 );
    u32 FrameCount = 0;

    // Initialize resources necessary for parallel change distribution.
    TaskManager::getInstance().PerThreadCallback( m_pObjectCCM->InitThreadLocalData, m_pObjectCCM );    
    TaskManager::getInstance().PerThreadCallback( m_pSceneCCM->InitThreadLocalData, m_pSceneCCM ); 

    while ( m_bExecuteLoop )
    {
        // Call the scheduler to have the systems internally update themselves.
        m_pScheduler->Execute();

        // Set any properties that may have been issued for change.  Any propeties that correlate
        // to system change notifications will be added to the change controller by the system.
        // NOTE: This is still untested as noone is using it.
        IssuePendingSystemPropertyChanges();

        // Distribute changes for object and scene CCMs.  The UObject propagates some object
        // messages up to the scene CCM so it needs to go first.
        m_pObjectCCM->DistributeQueuedChanges();
        m_pSceneCCM->DistributeQueuedChanges();

        // Check with the environment manager if there is a change in the runtime status to quit.
        if ( EnvironmentManager::getInstance().Runtime().GetStatus() ==
             IEnvironment::IRuntime::Status::Quit )
        {
            // Time to quit looping.
            m_bExecuteLoop = False;
        }

        // Increment the frame count and max frame limit for exit.
        FrameCount++;
        if ( StopAfterNFrames != 0 && FrameCount >= StopAfterNFrames )
        {
            // Time to quit looping.
            m_bExecuteLoop = False;
        }
    }

    return Errors::Success;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// IService::ISystemAccess Implementations.

Handle
Framework::GetSystem( pcstr pszSystemName)
{
    /* Get the pointer to the system from the system manager.  Handle is just a void* so it will
     * convert with any problems.  If you're one of those guys that will think of just casting
     * the handle back you'd better not as you'll break the threading when you try to make calls
     * into this interface directly.*/
    return SystemManager::getInstance().Get( pszSystemName );
}


Handle
Framework::GetSystem( System::Type Type)
{

    /* Get the pointer to the system from the system manager.  Handle is just a void* so it will
     * convert with any problems.  If you're one of those guys that will think of just casting
     * the handle back you'd better not as you'll break the threading when you try to make calls
     * into this interface directly.*/
    return SystemManager::getInstance().Get( Type );
}


Handle
Framework::GetScene( pcstr pszSystemName)
{
    Handle hScene = NULL;

    // Get the system from the system manager to get the type.
    ISystem* pSystem = SystemManager::getInstance().Get( pszSystemName );

    if ( pSystem != NULL )
    {
        // Get the scene based on the type.
        hScene = GetScene( pSystem->GetSystemType() );
    }

    return hScene;
}


Handle
Framework::GetScene( System::Type Type)
{
    Handle hScene = NULL;

    // Find the scene extension in the universal scene.
    UScene::SystemScenes::const_iterator it = m_pScene->GetSystemScenes().find( Type );

    if ( it != m_pScene->GetSystemScenes().end() )
    {
        hScene = it->second;
    }

    return hScene;
}


Handle
Framework::GetSystemObject( pcstr pszSystemName, pcstr pszName)
{
    Handle hObject = NULL;

    // Get the system from the system manager to get the type.
    ISystem* pSystem = SystemManager::getInstance().Get( pszSystemName );

    if ( pSystem != NULL )
    {
        hObject = GetSystemObject( pSystem->GetSystemType(), pszName );
    }

    return hObject;
}


Handle
Framework::GetSystemObject( System::Type Type, pcstr pszName)
{
    Handle hObject = NULL;

    // Find the universal object in the scene.
    UObject* pUObject = m_pScene->FindObject( pszName );

    if ( pUObject != NULL )
    {
        // Get the system object extension of the universal object using the system type.
        ISystemObject* pObject = pUObject->GetExtension( Type );

        if ( pObject != NULL )
        {
            hObject = pObject;
        }
    }

    return hObject;
}


void
Framework::GetSystemProperty( Handle hSystem, InOut Properties::Property& Property)
{
    std::string sPropertyName = Property.GetName();

    // Reinterpret the handle as an ISystem.
    if ( hSystem == NULL)
    {
        std::cerr << "hSystem == NULL" << std::endl;
    }
    ISystem* pSystem = reinterpret_cast<ISystem*>(hSystem);

    // Get the properties and find a match.
    Properties::Array aProperties;
    pSystem->GetProperties( aProperties );

    bool bFound = false;
    for ( Properties::ConstIterator it=aProperties.begin(); it != aProperties.end(); it++ )
    {
        if ( sPropertyName == it->GetName() )
        {
            Property = *it;
            bFound = true;
        }
    }
    if ( bFound == false)
    {
        std::cerr << "The property does not exist." << std::endl;
    }
}


void
Framework::SetSystemProperty(
    Handle hSystem,
    In Properties::Property& Property
    )
{
    // Reinterpret the handle as an ISystem.
    if ( hSystem == NULL)
    {
        std::cerr << "hSystem == NULL" << std::endl;
    }
    ISystem* pSystem = reinterpret_cast<ISystem*>(hSystem);

#ifdef _DEBUG
    // This will cause an assertion if the property doesn't exist.
    //Properties::Property TempProperty = Property;
    //GetSystemProperty( hSystem, TempProperty );
#endif

    PropertyIssue pi = { System::System, pSystem->GetSystemType(), hSystem, Property };

    std::lock_guard<std::mutex> lock(m_PropertyIssuesMutex);
    m_PropertyIssues.push_back( pi );
}


void
Framework::GetSceneProperty(
    Handle hScene,
    InOut Properties::Property& Property
    )
{
    std::string sPropertyName = Property.GetName();

    // Reinterpret the handle as an ISystem.
    if ( hScene == NULL)
    {
        std::cerr << "hScene == NULL" << std::endl;
    }
    ISystemScene* pSystemScene = reinterpret_cast<ISystemScene*>(hScene);

    // Get the properties and find a match.
    Properties::Array aProperties;
    pSystemScene->GetProperties( aProperties );

    Bool bFound = False;
    for ( Properties::ConstIterator it=aProperties.begin(); it != aProperties.end(); it++ )
    {
        if ( sPropertyName == it->GetName() )
        {
            Property = *it;
            bFound = True;
        }
    }
    if ( bFound == false)
    {
        std::cerr << "The property does not exist." << std::endl;
    }
}


void
Framework::SetSceneProperty(
    Handle hScene,
    In Properties::Property& Property
    )
{
    // Reinterpret the handle as an ISystemScene.
    if ( hScene == NULL)
    {
        std::cerr << "hScene == NULL" << std::endl;
    }
    ISystemScene* pSystemScene = reinterpret_cast<ISystemScene*>(hScene);


#ifdef _DEBUG
    // This will cause an assertion if the property doesn't exist.
    //Properties::Property TempProperty = Property;
    //GetSceneProperty( hScene, TempProperty );
#endif

    PropertyIssue pi = { System::Scene, pSystemScene->GetSystemType(),
                         hScene, Property };

    std::lock_guard<std::mutex> lock(m_PropertyIssuesMutex);
    m_PropertyIssues.push_back( pi );
}


void
Framework::GetObjectProperty(
    Handle hObject,
    InOut Properties::Property& Property
    )
{
    std::string sPropertyName = Property.GetName();

    // Reinterpret the handle as an ISystemScene.
    if ( hObject == NULL)
    {
        std::cerr << "hObject == NULL" << std::endl;
    }
    ISystemObject* pSystemObject = reinterpret_cast<ISystemObject*>(hObject);

    //
    // Get the properties and find a match.
    //
    Properties::Array aProperties;
    pSystemObject->GetProperties( aProperties );

    Bool bFound = False;
    for ( Properties::ConstIterator it=aProperties.begin(); it != aProperties.end(); it++ )
    {
        if ( sPropertyName == it->GetName() )
        {
            Property = *it;
            bFound = True;
        }
    }
    if ( bFound == false)
    {
        std::cerr << "The property does not exist." << std::endl;
    }
}


void
Framework::SetObjectProperty(
    Handle hObject,
    In Properties::Property& Property
    )
{
    // Reinterpret the handle as an ISystemObject.
    if ( hObject == NULL)
    {
        std::cerr << "hObject == NULL" << std::endl;
    }
    ISystemObject* pSystemObject = reinterpret_cast<ISystemObject*>(hObject);

#ifdef _DEBUG
    // This will cause an assertion if the property doesn't exist.
    //Properties::Property TempProperty = Property;
    //GetObjectProperty( hObject, TempProperty );
#endif

    PropertyIssue pi = { System::Object, pSystemObject->GetSystemType(),
                         hObject, Property };

    std::lock_guard<std::mutex> lock(m_PropertyIssuesMutex);
    m_PropertyIssues.push_back( pi );
}


void
Framework::IssuePendingSystemPropertyChanges(
    System::Types::BitMask SystemTypes
    )
{
    //
    // Create an array used for setting the property.
    //
    Properties::Array aProperties( 1 );

    //
    // Iterate through all the properties to issue.
    //
    for ( PropertyIssuesIt it=m_PropertyIssues.begin(); it != m_PropertyIssues.end(); it++ )
    {
        //
        // Get the property to issue.
        //
        const PropertyIssue& pi = *it;

        //
        // Check if the schuler will allow issuing the property change.
        //
        if ( pi.SystemType & SystemTypes )
        {
            aProperties[ 0 ] = pi.Property;

            switch ( pi.SystemComponent )
            {
            case System::System:
                reinterpret_cast<ISystem*>(pi.hItem)->SetProperties( aProperties );
                break;

            case System::Scene:
                reinterpret_cast<ISystemScene*>(pi.hItem)->SetProperties( aProperties );
                break;

            case System::Object:
                reinterpret_cast<ISystemObject*>(pi.hItem)->SetProperties( aProperties );
                break;

            default:
                std::cerr << "Unhandled case." << std::endl;
                break;
            };
        }
    }

    m_PropertyIssues.clear();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Framework::GDFParser Implementations.
Framework::GDFParser::GDFParser(
    UScene* pScene,
    std::string sOldpath
    )
    : m_pScene( pScene ),
      m_sOldpath( sOldpath )
{
}


void
Framework::GDFParser::ParseEnvironment( std::string sGDF)
{
    // Load the gdf xml file.
    TiXmlDocument   XmlDoc( sGDF.c_str() );

    if ( XmlDoc.LoadFile() == False )
    {
        std::cerr << "Parser was unable to load GDF file " << sGDF.c_str() << "." << std::endl;
    }

    // Find the "environment" element.
    TiXmlElement* pElement = XmlDoc.FirstChildElement();
    if ( pElement == NULL )
    {
        std::cerr << "Parser could not load the first element in the GDF." << std::endl;
    }
    if (strcmp( pElement->Value(), "GlobalDefinition" ) != 0)
    {
        std::cerr << "Parser did not see <GlobablDefinition> as the root element instead it was " 
                    <<  pElement->Value() << "." << " File " << sGDF.c_str() << ", Row " 
                    <<  pElement->Row() << ", Column " << pElement->Column() << "." << std::endl;
    }

    pElement = pElement->FirstChildElement();

    while ( pElement != NULL && strcmp( pElement->Value(), "Environment" ) != 0 )
    {
        pElement = pElement->NextSiblingElement();
    }

    if ( pElement != NULL )
    {
        m_GdfMarker = GDFM_Gdf;
        ParseElement( pElement, False );
    }
}


const std::string 
Framework::GDFParser::Parse( std::string sGDF)
{
    // Load the gdf xml file.
    TiXmlDocument   XmlDoc( sGDF.c_str() );

    if ( XmlDoc.LoadFile() == False )
    {
        std::cerr << "Parser was unable to load GDF file " << sGDF.c_str() << "." << std::endl;
    }

    // Set all the markers to false.
    m_GdfMarker = GDFM_None;

    // Get each element and attribute.
    TiXmlElement* pElement = XmlDoc.FirstChildElement();
    if ( pElement == NULL )
    {
        std::cerr << "Parser could not load the first element in the GDF." << std::endl;
    }

    ParseElement( pElement );

    return m_sNextScene;
}


const std::string
Framework::GDFParser::ParseScene( std::string sGDF, std::string sScene)
{
    if ( sScene.empty() == True )
    {
        std::cerr << "Parser does not have a scene to parse." << std::endl;
    }

    // Load the gdf xml file.
    TiXmlDocument   XmlDoc( sGDF.c_str() );

    if ( XmlDoc.LoadFile() == False )
    {
        std::cerr << "Parser was unable to load GDF file " << sGDF.c_str() << "."<< std::endl;
    }

    // Find the "scenes" element.
    TiXmlElement* pElement = XmlDoc.FirstChildElement();
    if ( pElement == NULL )
    {
        std::cerr << "Parser could not load the first element in the GDF." << std::endl;
    }
    if (strcmp( pElement->Value(), "GlobalDefinition" ) != 0)
    {
        std::cerr << "Parser did not see <GlobablDefinition> as the root element instead it was " 
                    <<  pElement->Value() << "." << " File " << sGDF.c_str() << ", Row " 
                    <<  pElement->Row() << ", Column " << pElement->Column() << "." << std::endl;
    }

    pElement = pElement->FirstChildElement();

    while ( pElement != NULL && strcmp( pElement->Value(), "Scenes" ) != 0 )
    {
        pElement = pElement->NextSiblingElement();
    }

    if ( pElement != NULL )
    {
        m_FirstObjectsMarker = True;

        // Find the selected scene.
        pElement = pElement->FirstChildElement();
        if ( pElement == NULL )
        {
            std::cerr << "Parser could not load the first element in the GDF." << std::endl;
        }
        if (strcmp( pElement->Value(), "Scene" ) != 0)
        {
            std::cerr << "Parser did not see <Scene> element instead it was " 
                        <<  pElement->Value() << "." << " File " << sGDF.c_str() << ", Row " 
                        <<  pElement->Row() << ", Column " << pElement->Column() << "." 
                        << std::endl;
        }

        while ( pElement != NULL )
        {
            pcstr pszName = pElement->Attribute( "Name" );

            if ( pszName != NULL && sScene == pszName )
            {
                m_GdfMarker = GDFM_Scenes;
                m_SceneLevel = 0;

                ParseElement( pElement );

                break;
            }

            pElement = pElement->NextSiblingElement();
        }
    }

    return m_sNextScene;
}


void
Framework::GDFParser::ParseElement( void* pElement, Bool bAllowProcessSiblings)
{
    TiXmlElement* pXmlElement = reinterpret_cast<TiXmlElement*>(pElement);

    // Do beginning element sequence.
    Bool bProcessChildren = BeginElement( pXmlElement );

    // Get the element's attributes and parse them.
    TiXmlAttribute* pXmlAttrib = pXmlElement->FirstAttribute();

    if ( pXmlAttrib != NULL )
    {
        ReadAttributes( pXmlElement, pXmlElement->FirstAttribute() );
    }

    if ( bProcessChildren )
    {
        // Get the element's first child and parse it.
        TiXmlElement* pXmlChildElement = pXmlElement->FirstChildElement();

        if ( pXmlChildElement != NULL )
        {
            ParseElement( pXmlChildElement );
        }
    }

    // Do ending element sequence.
    Bool bProcessSiblings = EndElement( pXmlElement );

    if ( bProcessSiblings && bAllowProcessSiblings )
    {
        // Get the next element and parse it.
        TiXmlElement* pXmlNextElement = pXmlElement->NextSiblingElement();

        if ( pXmlNextElement != NULL )
        {
            ParseElement( pXmlNextElement );
        }
    }
}


Bool
Framework::GDFParser::BeginElement( void* pElement)
{
    Bool bProcessChildren = True;

    TiXmlElement* pXmlElement = reinterpret_cast<TiXmlElement*>(pElement);

    pcstr pszName = pXmlElement->Value();

    if ( strcmp( pszName, "GlobalDefinition" ) == 0 )
    {
        if (m_GdfMarker != GDFM_None)
        {
            std::cerr << "Parser identified <GlobalDefinition> as not being at the root." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_GdfMarker = GDFM_Gdf;
    }
    else if ( strcmp( pszName, "Environment" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Gdf)
        {
            std::cerr << "Parser identified <Environment> as not being under <GlobalDefinition>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_GdfMarker = GDFM_Environment;
    }
    else if ( strcmp( pszName, "Variable" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Environment)
        {
            std::cerr << "Parser identified <Variable> as not being under <Environment>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_GdfMarker = GDFM_EnvVar;
    }
    else if ( strcmp( pszName, "Systems" ) == 0 )
    {
        if (m_GdfMarker !=GDFM_Gdf)
        {
            std::cerr << "Parser identified <Systems> as not being under <GlobalDefinition>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_SystemLevel = 0;

        m_GdfMarker = GDFM_Systems;
    }
    else if ( strcmp( pszName, "System" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Systems)
        {
            std::cerr << "Parser identified <System> as not being under <Systems>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        if ( m_SystemLevel == 0 )
        {
            m_pszSystemType = "";
            m_pSystem = NULL;
        }
        if (m_pSystem != NULL)
        {
            std::cerr << "Parser identified an error with using the CDF." << std::endl;
        }

        m_GdfMarker = GDFM_System;
    }
    else if ( strcmp( pszName, "Scenes" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Gdf)
        {
            std::cerr << "Parser identified <Scenes> as not being under <GlobalDefinition>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }
        else
        {
            bProcessChildren = False;

            m_GdfMarker = GDFM_Scenes;
        }
    }
    else if ( strcmp( pszName, "Scene" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scenes)
        {
            std::cerr << "Parser identified <Scene> as not being under <Scenes>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        if ( m_SceneLevel == 0 )
        {
            //
            // Create the initial scene for each system.
            //
            ISystem* pSystem = SystemManager::getInstance().GetFirst();

            while ( pSystem != NULL )
            {
                m_pScene->Extend( pSystem );

                pSystem = SystemManager::getInstance().GetNext();
            }

            //
            // Remove all the object properties.
            //
            m_AllObjectProperties.clear();
        }

        m_GdfMarker = GDFM_Scene;
    }
    else if ( strcmp( pszName, "Objects" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scene)
        {
            std::cerr << "Parser identified <Objects> as not being under <Scene>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_ObjectLevel = 0;

        m_GdfMarker = GDFM_Objects;

        //
        // Send "pre-loading objects" message to the scene extensions.
        //
        if ( m_FirstObjectsMarker )
        {
            m_FirstObjectsMarker = False;

            const UScene::SystemScenes Scenes = m_pScene->GetSystemScenes();
            for ( UScene::SystemScenesConstIt it=Scenes.begin(); it != Scenes.end(); it++ )
            {
                it->second->GlobalSceneStatusChanged(
                    ISystemScene::GlobalSceneStatus::PreLoadingObjects
                    );
            }
        }
    }
    else if ( strcmp( pszName, "Object" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Objects)
        {
            std::cerr << "Parser identified <Object> as not being under <Objects>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }
        else
        {
            if ( m_ObjectLevel == 0 )
            {
                m_pszObjectName = NULL;

                //
                // Create the object and add the required geometry extension.
                //
                m_pUObject = m_pScene->CreateObject();
                if (m_pUObject == NULL)
                {
                    std::cerr << "m_pUObject == NULL" << std::endl;
                }

                UScene::SystemScenesConstIt it = m_pScene->GetSystemScenes().find( System::Types::Geometry );
                    
                if (it == m_pScene->GetSystemScenes().end())
                {
                    std::cerr << "The geometry system has to have already been loaded." << std::endl;
                }

                ISystemScene* pGeometryScene = it->second;
                if (pGeometryScene == NULL)
                {
                    std::cerr << "pGeometryScene == NULL" << std::endl;
                }

                m_pUObject->Extend( pGeometryScene, NULL );

                //
                // Ready the propeties map for setting the properties.
                //
                m_SetPropertiesMap.clear();

                m_SetPropertiesMap[ System::Types::Geometry ] = Properties::Array();
            }

            m_GdfMarker = GDFM_Object;
        }
    }
    else if ( strcmp( pszName, "Properties" ) == 0 )
    {
        if (m_GdfMarker != GDFM_System && m_GdfMarker != GDFM_Scene && m_GdfMarker != GDFM_Object)
        {
            std::cerr << "Parser identified <Properties> as not being under <System> or <Scene> or <Object>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }
        else
        {
            if ( m_GdfMarker == GDFM_System )
            {
                if (m_pSystem == NULL)
                {
                    std::cerr << "Parser could not find an instantiated system." << std::endl;
                }
                else
                {
                    m_GdfMarker = GDFM_SystemProperties;
                }
            }
            else if ( m_GdfMarker == GDFM_Scene )
            {
                if (m_pSystem == NULL)
                {
                    std::cerr << "Parser could not find an instantiated system." << std::endl;
                }
                else
                {
                    m_GdfMarker = GDFM_SceneProperties;
                }
            }
            else if ( m_GdfMarker == GDFM_Object )
            {
                m_pszObjectType = NULL;
                m_pSystemObject = NULL;

                m_GdfMarker = GDFM_ObjectProperties;
            }
        }
    }
    else if ( strcmp( pszName, "Property" ) == 0 )
    {
        if (m_GdfMarker != GDFM_SystemProperties && m_GdfMarker != GDFM_SceneProperties &&
                    m_GdfMarker != GDFM_ObjectProperties)
        {
            std::cerr << "Parser identified <Property> as not being under <Properties> for"
                        << " <System> or <Scene> or <Object>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }
        else
        {
            if ( m_GdfMarker == GDFM_SystemProperties )
            {
                m_GdfMarker = GDFM_SystemProperty;
            }
            else if ( m_GdfMarker == GDFM_SceneProperties )
            {
                m_GdfMarker = GDFM_SceneProperty;
            }
            else if ( m_GdfMarker == GDFM_ObjectProperties )
            {
                m_GdfMarker = GDFM_ObjectProperty;
            }
        }
    }
    else if ( strcmp( pszName, "Links" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scene)
        {
            std::cerr << "Parser identified <Links> as not being under <Scene>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Links;
        }
    }
    else if ( strcmp( pszName, "Link" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Links)
        {
            std::cerr << "Parser identified <Link> as not being under <Links>." 
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        m_GdfMarker = GDFM_Link;
    }
    else if ( strcmp( pszName, "Include" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scene)
        {
            std::cerr << "Parser identified <Include> as not being under <Scene>."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }


        m_GdfMarker = GDFM_Include;
    }
    else
    {
        std::cerr << "Parser came across an unrecognized start marker " <<  pszName << "."
                    << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                    << std::endl;
    }


    return bProcessChildren;
}


Bool
Framework::GDFParser::EndElement(
    void* pElement
    )
{
    Bool bProcessSiblings = True;

    TiXmlElement* pXmlElement = reinterpret_cast<TiXmlElement*>(pElement);

    pcstr pszName = pXmlElement->Value();

    if ( strcmp( pszName, "GlobalDefinition" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Gdf)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_None;
        }
    }
    else if ( strcmp( pszName, "Environment" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Environment)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Gdf;
        }
    }
    else if ( strcmp( pszName, "Variable" ) == 0 )
    {
        if (m_GdfMarker != GDFM_EnvVar)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Environment;
        }
    }
    else if ( strcmp( pszName, "Systems" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Systems)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Gdf;
        }
    }
    else if ( strcmp( pszName, "System" ) == 0 )
    {
        if (m_GdfMarker != GDFM_System && m_GdfMarker != GDFM_SystemProperties)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            if ( m_SystemLevel == 0 )
            {
                m_pSystem->Initialize( m_SetProperties );
                m_GetProperties.clear();
                m_SetProperties.clear();
            }

            m_GdfMarker = GDFM_Systems;
        }
    }
    else if ( strcmp( pszName, "Scenes" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scenes)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Gdf;
        }
    }
    else if ( strcmp( pszName, "Scene" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Scene)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            bProcessSiblings = False;

            m_GdfMarker = GDFM_Scenes;

            //
            // Initialize all the objects and Send "post-loading objects" message to the scene extensions.
            //
            if ( !m_FirstObjectsMarker && m_SceneLevel < 2 )
            {
                m_FirstObjectsMarker = True;

                //
                // Initialize all the objects at once.
                //
                for ( std::map< UObject*,std::map<u32,Properties::Array> >::iterator
                       itAll=m_AllObjectProperties.begin();
                      itAll != m_AllObjectProperties.end(); itAll++ )
                {
                    UObject* pUObject = itAll->first;

                    for ( std::map<u32,Properties::Array>::iterator it = itAll->second.begin();
                          it != itAll->second.end(); it++ )
                    {
                        //
                        // Get the extension.
                        //
                        ISystemObject* pObject = pUObject->GetExtension( it->first );
                        if ( pObject == NULL)
                        {
                            std::cerr << "pObject == NULL" << std::endl;
                        }

                        //
                        // Initialize the extension.
                        //
                        if ( pObject != NULL )
                        {
                            pObject->Initialize( it->second );
                        }

                        //
                        // Remove all the properties.
                        //
                        it->second.clear();
                    }
                }
                m_AllObjectProperties.clear();

                const UScene::SystemScenes Scenes = m_pScene->GetSystemScenes();
                for ( UScene::SystemScenesConstIt it=Scenes.begin(); it != Scenes.end(); it++ )
                {
                    it->second->GlobalSceneStatusChanged(
                        ISystemScene::GlobalSceneStatus::PostLoadingObjects
                        );
                }
            }
        }
    }
    else if ( strcmp( pszName, "SystemProperties" ) == 0 )
    {
        if (m_GdfMarker != GDFM_SceneProperties)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Scene;
        }
    }
    else if ( strcmp( pszName, "Objects" ) == 0 )
    {
        if (m_GdfMarker != GDFM_Objects)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Scene;
        }
    }
    else if ( strcmp( pszName, "Object" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_Object && m_GdfMarker != GDFM_ObjectProperties)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            if ( m_ObjectLevel == 0 )
            {
                if ( m_pUObject == NULL)
                {
                    std::cerr << "A UObject should have already been created by this point." << std::endl;
                }

                //
                // Add this object's properties to the global collection.
                //
                m_AllObjectProperties[ m_pUObject ] = m_SetPropertiesMap;
                m_SetPropertiesMap.clear();

                //
                // We're done manipulating this object.
                //
                m_pUObject = NULL;
            }

            m_GdfMarker = GDFM_Objects;
        }
    }
    else if ( strcmp( pszName, "Properties" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_SystemProperties && m_GdfMarker != GDFM_SceneProperties && m_GdfMarker != GDFM_ObjectProperties)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            if ( m_GdfMarker == GDFM_SystemProperties )
            {
                m_GdfMarker = GDFM_System;
            }
            else if ( m_GdfMarker == GDFM_SceneProperties )
            {
                if ( m_SceneLevel > 2)
                {
                    std::cerr << "Scene properties are not valid for included CDFs."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    if ( m_pSystemScene == NULL)
                    {
                        std::cerr << "m_pSystemScene == NULL" << std::endl;
                    }
                    else
                    {
                        m_pSystemScene->Initialize( m_SetProperties );
                        m_GetProperties.clear();
                        m_SetProperties.clear();

                        m_GdfMarker = GDFM_Scene;
                    }
                }
            }
            else if ( m_GdfMarker == GDFM_ObjectProperties )
            {
                if ( m_pSystemObject == NULL)
                {
                    std::cerr << "m_pSystemObject == NULL" << std::endl;
                }
                else
                {
                    m_GetProperties.clear();
                    m_GdfMarker = GDFM_Object;
                }
            }
        }
    }
    else if ( strcmp( pszName, "Property" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_SystemProperty && m_GdfMarker != GDFM_SceneProperty && m_GdfMarker != GDFM_ObjectProperty)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            if ( m_GdfMarker == GDFM_SystemProperty )
            {
                m_GdfMarker = GDFM_SystemProperties;
            }
            else if ( m_GdfMarker == GDFM_SceneProperty )
            {
                m_GdfMarker = GDFM_SceneProperties;
            }
            else if ( m_GdfMarker == GDFM_ObjectProperty )
            {
                m_GdfMarker = GDFM_ObjectProperties;
            }
        }
    }
    else if ( strcmp( pszName, "Links" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_Links)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }

        m_GdfMarker = GDFM_Scene;
    }
    else if ( strcmp( pszName, "Link" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_Link)
        {
            std::cerr << "Parser identified the <" << pszName 
                        << "> end marker to be in the wrong location."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Links;
        }
    }
    else if ( strcmp( pszName, "Include" ) == 0 )
    {
        if ( m_GdfMarker != GDFM_Include)
        {
            std::cerr << "Parser identified the <Include> end marker as not being under <Include>."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            m_GdfMarker = GDFM_Scene;
        }
    }
    else
    {
        std::cerr << "Parser came across an unrecognized end marker <" << pszName << ">."
                    << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                    << std::endl;
    }

    return bProcessSiblings;
}


void
Framework::GDFParser::ReadAttributes(
    void* pElement, 
    void* pAttributes
    )
{
    TiXmlElement* pXmlElement = reinterpret_cast<TiXmlElement*>(pElement);
    TiXmlAttribute* pXmlAttrib = reinterpret_cast<TiXmlAttribute*>(pAttributes);

    if ( m_GdfMarker == GDFM_System )
    {
        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();

            if ( strcmp( pszName, "Type" ) == 0 )
            {
                if ( m_SystemLevel != 0 && strcmp( m_pszSystemType, pXmlAttrib->Value() ) != 0)
                {
                    std::cerr << "Parser could not load the system " <<  pXmlAttrib->Value()
                                << ". (" << pXmlElement->GetDocument()->Value() << " ,row "  
                                << pXmlAttrib->Row() << " ,column " << pXmlAttrib->Column()
                                << ")" << std::endl;
                }
                //
                // Store the name of the library.
                //
                m_pszSystemType = pXmlAttrib->Value();
            }
            else if ( strcmp( pszName, "Lib" ) == 0 )
            {
                //
                // Load the system library.
                //
                SystemManager::getInstance().LoadSystemLibrary(std::string(pXmlAttrib->Value()), m_sOldpath, &m_pSystem);
                    
                if (m_pSystem == NULL)
                {
                    std::cerr << "Parser could not load the system " <<  pXmlAttrib->Value()
                                << ". (" << pXmlElement->GetDocument()->Value() << " ,row "  
                                << pXmlAttrib->Row() << " ,column " << pXmlAttrib->Column()
                                << ")" << std::endl;
                }

                //
                // Get the properties of the system.
                //
                m_pSystem->GetProperties( m_GetProperties );
                
                if (strcmp( m_pszSystemType, m_pSystem->GetName() ) != 0)
                {
                        std::cerr << "Parser identified an incorrect system type. It should be " 
                                << m_pszSystemType << "." << " It is " << m_pSystem->GetName()
                                << ". ("<< pXmlElement->GetDocument()->Value() << " ,row "  
                                << pXmlAttrib->Row() << " ,column " << pXmlAttrib->Column()
                                << ")" << std::endl;
                }
            }
            else if ( strcmp( pszName, "SDF" ) == 0 )
            {
                if ( m_pSystem != NULL )
                {
                    std::cerr << "m_pSystem != NULL"<< std::endl;
                }

                //
                // Create a new xml document for the sdf.
                //
                char    pszSDF[ 256 ];
                strcpy_s( pszSDF, sizeof pszSDF, pXmlAttrib->Value() );
                strcat_s( pszSDF, sizeof pszSDF, ".sdf" );

                TiXmlDocument XmlDoc( pszSDF );

                if ( XmlDoc.LoadFile() == False )
                {
                    std::cerr << "Parser failed to load the SDF file" << pszSDF << "." << std::endl;
                }

                //
                // Parse the sdf.
                //
                m_GdfMarker = GDFM_Systems;
                m_SystemLevel++;

                ParseElement( XmlDoc.FirstChildElement() );

                m_SystemLevel--;
                m_GdfMarker = GDFM_System;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else if ( m_GdfMarker == GDFM_Environment )
    {
        // <Environment> currently has no attributes so do nothing.
    }
    else if ( m_GdfMarker == GDFM_EnvVar )
    {
        pcstr pszVariableName = NULL;
        pcstr pszVariableValue = NULL;

        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();

            if ( strcmp( pszName, "Name" ) == 0 )
            {
                if ( pszVariableName != NULL)
                {
                    std::cerr << "Duplicate attribute found '" << pszName << "'."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }

                pszVariableName = pXmlAttrib->Value();
            }
            else if ( strcmp( pszName, "Value" ) == 0 )
            {
                if ( pszVariableValue != NULL)
                {
                    std::cerr << "Duplicate attribute found '" << pszName << "'."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }

                pszVariableValue = pXmlAttrib->Value();
            }
            else
            {
                std::cerr << "Parser encountered an unknown <Variable> attribute '" << pszName << "'."
                            << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
        if ( pszVariableName == NULL || pszVariableValue == NULL)
        {
            std::cerr << "pszVariableName == NULL || pszVariableValue == NULL" << std::endl;
        }

        EnvironmentManager::getInstance().Variables().Add( pszVariableName, pszVariableValue );
    }
    else if ( m_GdfMarker == GDFM_SystemProperty )
    {
        if ( m_GetProperties.empty() == True)
        {
            std::cerr << "Parser found an empty property element."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            ReadPropertyAttributes( pXmlElement, pXmlAttrib, m_GetProperties, m_SetProperties );
        }
    }
    else if ( m_GdfMarker == GDFM_Scenes )
    {
        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();

            if ( strcmp( pszName, "Startup" ) == 0 )
            {
                m_sNextScene = pXmlAttrib->Value();
            }
            else
            {
                std::cerr << "Parser encountered an unknown <Scenes> attribute '" << pszName << "'."
                            << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else if ( m_GdfMarker == GDFM_Scene )
    {
        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "Name" ) == 0 )
            {
                if ( m_SceneLevel > 2)
                {
                    std::cerr << "Scene names are only valid for non-included CDFs."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    if ( m_SceneLevel != 0 && strcmp( m_pszSceneName, pszValue ) != 0)
                    {
                        std::cerr << "Parser identified an incorrect scene name. It should be " << m_pszSceneName  
                                    << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                    << std::endl;
                    }
                    else
                    {
                        m_pszSceneName = pszValue;
                    }
                }
            }
            else if ( strcmp( pszName, "CDF" ) == 0 )
            {
                if ( m_SceneLevel > 1)
                {
                    std::cerr << "Scene CDFs are only valid from the GDFs." 
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    //
                    // Create a new xml document for the cdf.
                    //
                    char    pszCDF[ 256 ];
                    strcpy_s( pszCDF, sizeof pszCDF, pszValue );
                    strcat_s( pszCDF, sizeof pszCDF, ".cdf" );

                    TiXmlDocument XmlDoc( pszCDF );

                    if ( XmlDoc.LoadFile() == False )
                    {
                        std::cerr << "Parser was unable to load CDF file " << pszCDF << "." << std::endl;
                    }
                    else
                    {
                        //
                        // Parse the sdf.
                        //
                        m_GdfMarker = GDFM_Scenes;
                        m_SceneLevel++;

                        ParseElement( XmlDoc.FirstChildElement() );

                        m_SceneLevel--;
                        m_GdfMarker = GDFM_Scene;
                    }
                }
            }
            else if ( strcmp( pszName, "NextScene" ) == 0 )
            {
                if ( m_SceneLevel != 0)
                {
                    std::cerr << "Parser identified a " << pszName << " attribute within an SDF. This can only be set in the GDF."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }

                m_sNextScene = pszValue;
            }
            else
            {
                std::cerr << "Parser encountered an unknown <Scene> attribute " << pszName
                            << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else if ( m_GdfMarker == GDFM_SceneProperties )
    {
        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "SystemType" ) == 0 )
            {
                m_pSystem = SystemManager::getInstance().Get( pszValue );
                if ( m_pSystem == NULL)
                {
                    std::cerr << "Parser was unable to get system " << pszValue
                                << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    m_GetProperties.clear();
                    m_SetProperties.clear();

                    UScene::SystemScenesConstIt it = m_pScene->GetSystemScenes().find( m_pSystem->GetSystemType() );
                    if ( it == m_pScene->GetSystemScenes().end())
                    {
                        std::cerr << "Parser was unable to find a scene for system " << pszValue
                                    << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                    << std::endl;
                    }
                    
                    m_pSystemScene = it->second;
                    if ( m_pSystemScene == NULL)
                    {
                        std::cerr << "m_pSystemScene == NULL" << std::endl;
                    }

                    m_pSystemScene->GetProperties( m_GetProperties );
                }
            }
            else
            {
                 std::cerr << "Parser encountered an unknown scene <Properties> attribute " << pszName
                            << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
      
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else if ( m_GdfMarker == GDFM_SceneProperty )
    {
        if ( m_GetProperties.empty() == True)
        {
            std::cerr << "Parser found an empty property element."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        } 
        else
        {
            ReadPropertyAttributes( pXmlElement, pXmlAttrib, m_GetProperties, m_SetProperties );
        }
    }
    else if ( m_GdfMarker == GDFM_Object )
    {
        if ( m_pUObject == NULL)
        {
            std::cerr << "Parser does not have a UObject to work with."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        } 

        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "Name" ) == 0 )
            {
                if ( m_ObjectLevel == 0 )
                {
                    m_pszObjectName = pszValue;

                    if ( m_pszObjectName != NULL )
                    {
                        m_pUObject->SetName( m_pszObjectName );
                    }
                }
            }
            else if ( strcmp( pszName, "ODF" ) == 0 )
            {
                //
                // Create a new xml document for the sdf.
                //
                char    pszODF[ 256 ];
                strcpy_s( pszODF, sizeof pszODF, pszValue );
                strcat_s( pszODF, sizeof pszODF, ".odf" );

                TiXmlDocument XmlDoc( pszODF );

                if ( XmlDoc.LoadFile() == False )
                {
                    std::cerr <<  "Parser was unable to load ODF file " << pszODF << "." << std::endl;
                }
                else
                {
                    //
                    // Parse the sdf.
                    //
                    m_GdfMarker = GDFM_Objects;
                    m_ObjectLevel++;

                    ParseElement( XmlDoc.FirstChildElement() );

                    m_ObjectLevel--;
                    m_GdfMarker = GDFM_Object;
                }
            }
            else
            {
                std::cerr << "Parser encountered an unknown <Object> attribute " <<  pszName
                            << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else if ( m_GdfMarker == GDFM_ObjectProperties )
    {
        if ( m_pUObject == NULL)
        {
            std::cerr << "Parser does not have a UObject to work with."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        } 

        //
        // Clear the properties.
        //
        m_GetProperties.clear();
        m_SetProperties.clear();

        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "SystemType" ) == 0 )
            {
                m_pSystem = SystemManager::getInstance().Get( pszValue );
                if ( m_pSystem == NULL)
                {
                    std::cerr << "Parser was unable to get system " << pszValue
                                << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                } 
                else
                {
                    m_pSystemObject = m_pUObject->GetExtension( m_pSystem->GetSystemType() );

                    if ( m_pSystemObject != NULL )
                    {
                        m_pSystemObject->GetProperties( m_GetProperties );
                    }
                }
            }
            else if ( strcmp( pszName, "ObjectType" ) == 0 )
            {
                m_pszObjectType = pszValue;
            }
            else
            {
                std::cerr << "Parser encountered an unknown object <Properties> attribute " << pszName
                            << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }

        //
        // Create the object if there isn't one yet.
        //
        if ( m_pSystemObject == NULL )
        {
            if ( m_pSystem != NULL )
            {
                UScene::SystemScenesConstIt it =
                    m_pScene->GetSystemScenes().find( m_pSystem->GetSystemType() );
                if ( it == m_pScene->GetSystemScenes().end())
                {
                    std::cerr << "Parser was unable to find a scene for this system."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    m_pSystemObject = m_pUObject->Extend( it->second, m_pszObjectType );
                    if ( m_pSystemObject == NULL )
                    {
                        std::cerr << "m_pSystemObject == NULL" << std::endl;
                    }
                    else
                    {
                        m_pSystemObject->GetProperties( m_GetProperties );

                        //
                        // Create an entry in the set properties map.
                        //
                        m_SetPropertiesMap[ m_pSystem->GetSystemType() ] = Properties::Array();
                    }
                }
            }
        }
    }
    else if ( m_GdfMarker == GDFM_ObjectProperty )
    {
        if ( m_GetProperties.empty() == True)
        {
            std::cerr << "Parser found an empty property element."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else
        {
            //
            // Get the system object's type.
            //
            System::Type SystemType = m_pSystem->GetSystemType();

            //
            // If there isn't any entry in the properties map then create one.
            //
            std::map<u32,Properties::Array>::const_iterator it =
                m_SetPropertiesMap.find( SystemType );

            if ( it == m_SetPropertiesMap.end() )
            {
                m_SetPropertiesMap[ SystemType ] = Properties::Array();
            }

            //
            // Read in the properties.
            //
            ReadPropertyAttributes( pXmlElement, pXmlAttrib, m_GetProperties, m_SetPropertiesMap[ SystemType ] );
        }
    }
    else if ( m_GdfMarker == GDFM_Links )
    {
        // <Links> currently has no attributes so do nothing.
    }
    else if ( m_GdfMarker == GDFM_Link )
    {
        UObject* pSubject = NULL;
        UObject* pObserver = NULL;
        std::string sSystemSubject;
        std::string sSystemObserver;

        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "Subject" ) == 0 )
            {
                pSubject = m_pScene->FindObject( pszValue );
            }
            else if ( strcmp( pszName, "Observer" ) == 0 )
            {
                pObserver = m_pScene->FindObject( pszValue );
            }
            else if ( strcmp( pszName, "SubjectSystem" ) == 0 )
            {
                sSystemSubject = pszValue;
            }
            else if ( strcmp( pszName, "ObserverSystem" ) == 0 )
            {
                sSystemObserver = pszValue;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
        if ( pSubject == NULL)
        {
            std::cerr << "Parser did not find the UObject subject."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else if ( pObserver == NULL)
        {
            std::cerr << "Parser did not find the UObject observer."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else if ( sSystemSubject.empty() )
        {
            std::cerr << "Parser did not find the system subject."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else if ( sSystemObserver.empty() )
        {
            std::cerr << "Parser did not find the system observer."
                        << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                        << std::endl;
        }
        else 
        {
            ISystemObject* pSystemSubject = NULL;
            ISystemObject* pSystemObserver = NULL;

            ISystem* pSystem;

            //
            // Get the extension for the subject.
            //
            if ( !sSystemSubject.empty() )
            {
                pSystem = SystemManager::getInstance().Get( sSystemSubject.c_str() );
                if ( pSystem == NULL )
                {
                    std::cerr << "Parser identified system " << sSystemSubject.c_str() << " as not loaded."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    pSystemSubject = pSubject->GetExtension( pSystem->GetSystemType() );
                    if ( pSystemSubject == NULL )
                    {
                        std::cerr << "Parser identifed subject system as not extending subject object."
                                    << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                    << std::endl;
                    }
                }   
            }

            //
            // Get the extension for the object.
            //
            pSystem = SystemManager::getInstance().Get( sSystemObserver.c_str() );
            if ( pSystem == NULL )
            {
                std::cerr << "Parser identified system " << sSystemObserver.c_str() << " as not loaded."
                            << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }
            else
            {
                pSystemObserver = pObserver->GetExtension( pSystem->GetSystemType() );
                if ( pSystemObserver == NULL )
                {
                    std::cerr << "Parser identifed observer system as not extending observer object."
                                << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    //
                    // Call the scene to register the links.
                    //
                    if ( pSystemSubject != NULL )
                    {
                        m_pScene->CreateObjectLink( pSystemSubject, pSystemObserver );
                    }
                    else
                    {
                        m_pScene->CreateObjectLink( pSubject, pSystemObserver );
                    }
                }
            }
        }
    }
    else if ( m_GdfMarker == GDFM_Include )
    {
        while ( pXmlAttrib != NULL )
        {
            pcstr pszName = pXmlAttrib->Name();
            pcstr pszValue = pXmlAttrib->Value();

            if ( strcmp( pszName, "CDF" ) == 0 )
            {
                //
                // Create a new xml document for the cdf.
                //
                char    pszCDF[ 256 ];
                strcpy_s( pszCDF, sizeof pszCDF, pszValue );
                strcat_s( pszCDF, sizeof pszCDF, ".cdf" );

                TiXmlDocument XmlDoc( pszCDF );

                if ( XmlDoc.LoadFile() == False )
                {
                    std::cerr <<  "Parser was unable to load CDF file " << pszCDF << "." << std::endl;
                }
                else
                {
                    //
                    // Parse the sdf.
                    //
                    m_GdfMarker = GDFM_Scenes;
                    m_SceneLevel++;

                    ParseElement( XmlDoc.FirstChildElement() );

                    m_SceneLevel--;
                    m_GdfMarker = GDFM_Include;
                }
            }
            else
            {
                std::cerr <<  "Parser encountered an unknown <Include> attribute " << pszName
                            << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }

            pXmlAttrib = pXmlAttrib->Next();
        }
    }
    else
    {
        std::cerr << "Parser encountered an unsupported marker."
                    << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                    << std::endl;
    }
}


void
Framework::GDFParser::ReadPropertyAttributes(
    void* pElement,
    void* pAttributes,
    std::vector<Properties::Property>& GetProperties,
    std::vector<Properties::Property>& SetProperties
    ) const
{
    TiXmlElement* pXmlElement = reinterpret_cast<TiXmlElement*>(pElement);
    TiXmlAttribute* pXmlAttrib = reinterpret_cast<TiXmlAttribute*>(pAttributes);

    Properties::ConstIterator GetPropIt = GetProperties.end();
    size_t iProp = 0xFFFFFFFF;

    while ( pXmlAttrib != NULL )
    {
        pcstr pszName = pXmlAttrib->Name();

        if ( strcmp( pszName, "Name" ) == 0 )
        {
            //
            // Search for the name in the property array.
            //
            pcstr pszValue = pXmlAttrib->Value();

            for ( Properties::ConstIterator it=GetProperties.begin(); it != GetProperties.end(); it++ )
            {
                if ( strcmp( it->GetName(), pszValue ) == 0 )
                {
                    SetProperties.push_back( *it );
                    iProp = SetProperties.size() - 1;
                    GetPropIt = it;
                    break;
                }
            }
            
            if ( GetPropIt == GetProperties.end())
            {
                std::cerr << "Parser could not find the property named " << pszValue << " in the list given by the system."
                            << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }
        }
        else if ( _stricmp( pszName, GetPropIt->GetValueName( 0 ) ) == 0 ||
                  _stricmp( pszName, GetPropIt->GetValueName( 1 ) ) == 0 ||
                  _stricmp( pszName, GetPropIt->GetValueName( 2 ) ) == 0 ||
                  _stricmp( pszName, GetPropIt->GetValueName( 3 ) ) == 0 )
        {
            if ( GetPropIt == GetProperties.end())
            {
                std::cerr << "Parser encountered a property value " << pszName << " that occured before the property name."
                            << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                            << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                            << std::endl;
            }
            else
            {
                //
                // Determine the index of the value.
                //
                u32 iValue;
                for ( iValue=0; iValue < Properties::Values::Count; iValue++ )
                {
                    if ( _stricmp( pszName, GetPropIt->GetValueName( iValue ) ) == 0 )
                    {
                        break;
                    }
                }
                if ( iValue == Properties::Values::Count )
                {
                    std::cerr << "Parser encoutered an unknown value '" << pszName
                                << ". (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                << std::endl;
                }
                else
                {
                    //
                    // Read in the property from the xml.
                    //
                    switch ( GetPropIt->GetValueType( iValue ) )
                    {
                    case Properties::Values::None:
                        std::cerr << "Parser encoutered a value '" << pszName << "' with no type."
                                    << " (" << pXmlElement->GetDocument()->Value() << ", Row " 
                                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << ")" 
                                    << std::endl;
                        break;

                    case Properties::Values::Boolean:
                    case Properties::Values::Int32:
                        SetProperties[ iProp ].SetValue( iValue, pXmlAttrib->IntValue() );
                        break;

                    case Properties::Values::Enum:
                    {
                        pcstr pszEnumOption = pXmlAttrib->Value();

                        for ( i32 i=0; GetPropIt->GetEnumOption( i ) != NULL; i++ )
                        {
                            if ( strcmp( pszEnumOption, GetPropIt->GetEnumOption( i ) ) == 0 )
                            {
                                SetProperties[ iProp ].SetValue( iValue, i );
                                break;
                            }
                        }
                        break;
                    }

                    case Properties::Values::Float32:
                    case Properties::Values::Vector3 & Properties::Values::Mask:
                    case Properties::Values::Vector4 & Properties::Values::Mask:
                    case Properties::Values::Quaternion & Properties::Values::Mask:
                    case Properties::Values::Color3 & Properties::Values::Mask:
                    case Properties::Values::Color4 & Properties::Values::Mask:
                        SetProperties[ iProp ].SetValue( iValue,
                                                         static_cast<f32>(pXmlAttrib->DoubleValue()) );
                        break;

                    case Properties::Values::Angle:
                        SetProperties[ iProp ].SetValue(
                            iValue, Base::Angle::Deg2Rad( static_cast<f32>(pXmlAttrib->DoubleValue()) )
                            );
                        break;

                    case Properties::Values::String:
                    case Properties::Values::Path:
                        SetProperties[ iProp ].SetValue( iValue, pXmlAttrib->Value() );
                        break;

                    default:
                        std::cerr << "Parser encountered an unsupported property value."
                                    << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                                    << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                                    << std::endl;
                        break;
                    }
                }
            }
        }
        else
        {
            std::cerr << "Parser encountered an unknown property attribute " <<  pszName << "."
                        << " File " << pXmlElement->GetDocument()->Value() << ", Row " 
                        << pXmlElement->Row() << ", Column " << pXmlElement->Column() << "." 
                        << std::endl;
        }

        pXmlAttrib = pXmlAttrib->Next();
    }
}
