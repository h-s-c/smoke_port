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


enum ScriptChanges
{
    None            = 0x00000000,
    Position        = 0x00000001,
    Orientation     = 0x00000002,
    Scale           = 0x00000004
};

struct ScriptParameters
{
    int             HashCode;
    int             ExpectedChanges;
    int             Changes;
    float           Position[ 3 ];
    float           Orientation[ 4 ];
    float           Scale[ 3 ];
};


extern void InitializeScriptingEngine( const char** apszScriptsLocations, int cScripsLocations );

extern int LoadScript( const char* pszScriptFile, int& ExpectedChanges );

extern void UnloadScript( int hScript );

extern void UnloadAllScripts( void );

extern void ExecuteScripts( ScriptParameters aParams[], int cParams, float DeltaTime );
