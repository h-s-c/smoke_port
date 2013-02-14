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

#pragma once


public ref class ScriptBase abstract
{
public:

    static const int PositionChanged    = 0x00000001;
    static const int OrientationChanged = 0x00000002;
    static const int ScaleChanged       = 0x00000004;

    interface class IGeometry
    {
        virtual void GetPosition( float% x, float% y, float% z ) abstract;
        virtual void GetOrientation( float% x, float% y, float% z, float% w ) abstract;
        virtual void GetScale( float% x, float% y, float% z ) abstract;

        virtual void SetPosition( float x, float y, float z ) abstract;
        virtual void SetOrientation( float x, float y, float z, float w ) abstract;
        virtual void SetScale( float x, float y, float z ) abstract;
    };

    // <summary>
    //   Required implementation function for scripts to identify what changes it wants to recieve.
    // </summary>
    // <returns>A bit mask of the desired changes.</retuns>
    virtual int GetExpectedChanges( void ) abstract;

    // <summary>
    //   Required implementation function for scripts to receive notification to make updates.
    // </summary>
    // <param name="DeltaTime">The time between frames in seconds.</param>
    // <returns>The notification on the data that was changed.</returns>
    virtual int Update( float DeltaTime ) abstract;
};


private ref struct ScriptData
{
    int                 HashCode;
    Assembly^           pAssembly;
    ScriptBase^         pScriptBase;
};


private ref class ScriptingEngine
{
public:

    static void Initialize( void );

    static void AddScriptsLocation( String^ sScriptsLocation );

    static int LoadScript( String^ sScriptFile, int& ExpectedChanges );

    static void UnloadScript( int hScript );

    static void UnloadAllScripts( void );

    static void ExecuteScript( ScriptParameters& Params, float DeltaTime );

    static void ExecuteScripts( ScriptParameters aParams[], int cParams, float DeltaTime );


protected:

    static Collections::ArrayList                               sm_asScriptsLocations;
    static Collections::Generic::Dictionary<int, ScriptData^>   sm_aAssemblies;

    static const int                                            sm_UpdateArraySize = 8;
    static array<Object^>^                                      sm_aUpdateData;
};
