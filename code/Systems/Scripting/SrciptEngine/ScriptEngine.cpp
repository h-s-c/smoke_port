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

#pragma managed


//
// clr libraries
//
#using <mscorlib.dll>
#using <system.dll>
using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;

#include "ScriptEngineAPI.h"
#include "ScriptMath.h"
#include "ScriptEngine.h"


void
InitializeScriptingEngine(
    const char** apszScriptsLocations,
    int cScripsLocations
    )
{
    Diagnostics::Debug::Listeners->Add( gcnew Diagnostics::ConsoleTraceListener() );

    ScriptingEngine::Initialize();

    for ( int i=0; i < cScripsLocations; i++ )
    {
        String^ sScriptsLocation = gcnew String( apszScriptsLocations[ i ] );
        ScriptingEngine::AddScriptsLocation( sScriptsLocation );
    }
}


int
LoadScript(
    const char* pszScriptFile,
    int& ExpectedChanges
    )
{
    String^ sScriptFile = gcnew String( pszScriptFile );
    return ScriptingEngine::LoadScript( sScriptFile, ExpectedChanges );
}


void
UnloadScript(
    int hScript
    )
{
    ScriptingEngine::UnloadScript( hScript );
}


void
UnloadAllScripts(
    void
    )
{
    ScriptingEngine::UnloadAllScripts();
}


void
ExecuteScripts(
    ScriptParameters aParams[],
    int cParams,
    float DeltaTime
    )
{
    ScriptingEngine::ExecuteScripts( aParams, cParams, DeltaTime );
}


void
ScriptingEngine::Initialize(
    void
    )
{
    //
    // Create the security policy level for this dll.  This will remain throughout the
    //  run of a scene.
    //
    Security::Policy::PolicyLevel^ pPolicyLevel =
        Security::Policy::PolicyLevel::CreateAppDomainLevel();

    //
    // Create a permission set for executing the scripts.
    //
    Security::PermissionSet^ pPermissionSet =
        gcnew Security::PermissionSet( Security::Permissions::PermissionState::None );

    pPermissionSet->AddPermission(
        gcnew Security::Permissions::SecurityPermission(
            Security::Permissions::SecurityPermissionFlag::Execution
            )
        );
                
    pPermissionSet->Demand();

    //
    // Change the policy statement for our policy level based on the newly created
    //  permission set.
    //
    pPolicyLevel->RootCodeGroup->PolicyStatement =
        gcnew Security::Policy::PolicyStatement( pPermissionSet );

    AppDomain::CurrentDomain->SetAppDomainPolicy( pPolicyLevel );

    //
    // Allocate an array for holding the update data.
    //
    sm_aUpdateData = gcnew array<Object^>( sm_UpdateArraySize );
}


void
ScriptingEngine::AddScriptsLocation(
    String^ sScriptsLocation
    )
{
    if ( !sScriptsLocation->EndsWith( "\\" ) )
    {
        sScriptsLocation += "\\";
    }
    sm_asScriptsLocations.Add( sScriptsLocation );
}


int
ScriptingEngine::LoadScript(
    String^ sScriptFile,
    int& ExpectedChanges
    )
{
    int HashCode = 0;

    //
    // Adjust the script name if no .cs extension.
    //
    if ( !sScriptFile->EndsWith( ".cs" ) )
    {
        sScriptFile += ".cs";
    }

    //
    // Check if the file exists.  Use all the locations passed in to search.
    //
    String^ sScriptPathName = nullptr;

    for ( int i=0; i < sm_asScriptsLocations.Count; i++ )
    {
        String^ sPathName = IO::Path::GetFullPath( sm_asScriptsLocations[ i ] + sScriptFile );

        if ( IO::File::Exists( sPathName ) )
        {
            sScriptPathName = sPathName;
            break;
        }
    }

    Diagnostics::Debug::WriteIf(
        sScriptPathName == nullptr,
        "*** SystemScripting: the script '" + sScriptFile + "' was not found.\n"
        );

    if ( sScriptPathName != nullptr )
    {
        try
        {
            //
            // Instantiate the C# compiler.
            //
            Microsoft::CSharp::CSharpCodeProvider^ pCSCodeProvider =
                gcnew Microsoft::CSharp::CSharpCodeProvider();

            CodeDom::Compiler::CompilerParameters^ pCompilerParams =
                gcnew CodeDom::Compiler::CompilerParameters();

            pCompilerParams->GenerateExecutable = false;        // make an assembly
            pCompilerParams->GenerateInMemory = true;           // keep it in memory
#if defined( _DEBUG )
            pCompilerParams->IncludeDebugInformation = true;    // turn on debugging
#else
            pCompilerParams->CompilerOptions = "/optimize";
#endif
            pCompilerParams->ReferencedAssemblies->Add(
                System::Reflection::Assembly::GetExecutingAssembly()->Location
                );

            //
            // Compile the script.
            //
            CodeDom::Compiler::CompilerResults^ pCompilerResult =
                pCSCodeProvider->CompileAssemblyFromFile(
                    pCompilerParams, sScriptPathName
                    );

            if ( pCompilerResult->Errors->Count == 0 )
            {
                //
                // Get the compiled assembly.
                //
                Reflection::Assembly^ pAssembly = pCompilerResult->CompiledAssembly;

                //
                // Instantiate the class.
                //
                Type^ pType = pAssembly->GetType( "Script", false, true );

                ConstructorInfo^ pConstructorInfo = pType->GetConstructor( Type::EmptyTypes );

                ScriptBase^ pScriptBase =
                    dynamic_cast<ScriptBase^>(pConstructorInfo->Invoke( nullptr ));

                //
                // Now that the class is instantiated get the expected changes from it.
                //
                ExpectedChanges = pScriptBase->GetExpectedChanges();

                //
                // Get the hash code.
                //
                HashCode = pScriptBase->GetHashCode();

                //
                // Create the data for the script.
                //
                ScriptData^ sd = gcnew ScriptData;
                sd->HashCode = HashCode;
                sd->pAssembly = pAssembly;
                sd->pScriptBase = pScriptBase;

                sm_aAssemblies.Add( HashCode, sd );
            }
            else
            {
                Diagnostics::Debug::Write(
                    "*** SystemScripting: errors for script '" + sScriptFile + "'.\n"
                    );

                for ( int i=0; i < pCompilerResult->Errors->Count; i++ )
                {
                    Diagnostics::Debug::Write( pCompilerResult->Errors[ i ] + "\n" );
                }
            }
        }
        catch ( ... )
        {
            Diagnostics::Debug::Write(
                "*** SystemScripting: there was an error loading script '" + sScriptFile + "'.\n"
                );
        }
    }

    return HashCode;
}


void
ScriptingEngine::UnloadScript(
    int hScript
    )
{
    sm_aAssemblies.Remove( hScript );
}


void
ScriptingEngine::UnloadAllScripts(
    void
    )
{
    sm_aAssemblies.Clear();
}


void
ScriptingEngine::ExecuteScript(
    ScriptParameters& Params,
    float DeltaTime
    )
{
    ScriptData^ sd;

    if ( sm_aAssemblies.TryGetValue( Params.HashCode, sd ) )
    {
        if ( sd->HashCode != 0 )
        {
            ScriptBase^ pScriptBase = sd->pScriptBase;

            try
            {
                int ExpectedChanges = Params.ExpectedChanges;

                //
                // Set any data that may have changed.
                //
                if ( ExpectedChanges & Params.Changes & ScriptChanges::Position )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->SetPosition(
                        Params.Position[ 0 ], Params.Position[ 1 ], Params.Position[ 2 ]
                        );
                }

                if ( ExpectedChanges & Params.Changes & ScriptChanges::Orientation )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->SetOrientation(
                        Params.Orientation[ 0 ], Params.Orientation[ 1 ],
                        Params.Orientation[ 2 ], Params.Orientation[ 3 ]
                        );
                }

                if ( ExpectedChanges & Params.Changes & ScriptChanges::Scale )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->SetScale(
                        Params.Scale[ 0 ], Params.Scale[ 1 ], Params.Scale[ 2 ]
                        );
                }

                //
                // Call the script to update.
                //
                Params.Changes = pScriptBase->Update( DeltaTime );

                //
                // Get any data that may have changed.
                //
                if ( Params.Changes & ScriptChanges::Position )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->GetPosition(
                        Params.Position[ 0 ], Params.Position[ 1 ], Params.Position[ 2 ]
                        );
                }

                if ( Params.Changes & ScriptChanges::Orientation )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->GetOrientation(
                        Params.Orientation[ 0 ], Params.Orientation[ 1 ],
                        Params.Orientation[ 2 ], Params.Orientation[ 2 ]
                        );
                }

                if ( Params.Changes & ScriptChanges::Scale )
                {
                    dynamic_cast<ScriptBase::IGeometry^>(pScriptBase)->GetScale(
                        Params.Scale[ 0 ], Params.Scale[ 1 ], Params.Scale[ 2 ]
                        );
                }
            }
            catch ( ... )
            {
            }
        }
    }
}


void
ScriptingEngine::ExecuteScripts(
    ScriptParameters aParams[],
    int cParams,
    float DeltaTime
    )
{
    //
    // Process all the parameters.
    //
    for ( int i=0; i < cParams; i++ )
    {
        ExecuteScript( aParams[ i ], DeltaTime );
    }
}
