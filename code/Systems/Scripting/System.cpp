// Copyright © 2008 Intel Corporation
// All Rights Reserved
// 
// The sample source code contained or described herein and all documents
// related to the sample source code ("Material") are owned by Intel Corporation
// or its suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material may contain trade secrets and
// proprietary and confidential information of Intel Corporation and its suppliers
// and licensors, and is protected by worldwide copyright and trade secret laws and
// treaty provisions. The sample source code is provided AS IS, with no warranties
// of any kind, express or implied. Except as expressly permitted by the Software
// license, neither Intel Corporation nor its suppliers assumes any responsibility
// or liability for any errors or inaccuracies that may appear herein.

#pragma unmanaged


//
// core includes
//
#pragma warning( disable : 4633 )
#include "..\BaseTypes\BaseTypes.h"
#include "..\Interfaces\Interface.h"

#include "ScriptEngineAPI.h"

//
// system includes
//
#include "System.h"
#include "Scene.h"


pcstr ScriptingSystem::sm_kapszPropertyNames[] =
{
    "ScriptsLocation",
};

Properties::Property ScriptingSystem::sm_kaDefaultProperties[ Property_Count ];


ScriptingSystem::ScriptingSystem(
    void
    )
    : ISystem()
{
    //
    // Some weirdness w/ the mixed clr doesn't allow this to be statically initialized.
    //
    sm_kaDefaultProperties[ 0 ] =
        Properties::Property( sm_kapszPropertyNames[ Property_ScriptsLocation ],
                              VALUE1( Properties::Values::String ),
                              Properties::Flags::Valid | Properties::Flags::InitOnly |
                                Properties::Flags::Multiple,
                              "Path", NULL, NULL, NULL,
                              "" );

    ASSERT( Property_Count == (sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ]) );
    ASSERT( Property_Count == (sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ]) );
}


ScriptingSystem::~ScriptingSystem(
    void
    )
{
}


pcstr
ScriptingSystem::GetName(
    void
    )
{
    return Interface::System::Names::Scripting;
}


Interface::System::Type
ScriptingSystem::GetSystemType(
    void
    )
{
    return Interface::System::Types::Scripting;
}


Error
ScriptingSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    i32 cScriptsLocations = 0;
    pcstr apszScriptsLocations[ 20 ];

    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        //
        // Make sure this property is valid.
        //
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_ScriptsLocation ] )
            {
                ASSERT( cScriptsLocations < 20 );
                apszScriptsLocations[ cScriptsLocations++ ] = it->GetStringPtr( 0 );

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    InitializeScriptingEngine( apszScriptsLocations, cScriptsLocations );

	m_bInitialized = True;

    return Errors::Success;
}


void
ScriptingSystem::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }
}


void
ScriptingSystem::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
}


ISystemScene*
ScriptingSystem::CreateScene(
    void
    )
{
    return new ScriptingScene( this );
}


Error
ScriptingSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    ScriptingScene* pScene = reinterpret_cast<ScriptingScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}
