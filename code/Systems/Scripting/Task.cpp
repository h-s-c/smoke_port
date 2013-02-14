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
#include "Scene.h"
#include "Object.h"
#include "Task.h"


ScriptingTask::ScriptingTask(
	ScriptingScene* pScene
    )
    : ISystemTask( reinterpret_cast<ISystemScene*>(pScene) )
{
}


ScriptingTask::~ScriptingTask(
    void
    )
{
}


Interface::System::Type
ScriptingTask::GetSystemType(
    void
    )
{
    return Interface::System::Types::Scripting;
}


void
ScriptingTask::Update(
    f32 DeltaTime
    )
{
    //
    // Get the system scene.
    //
    ScriptingScene* pScene = reinterpret_cast<ScriptingScene*>(m_pSystemScene);

    ScriptParameters* aParams = pScene->m_aScriptParameters;

    //
    // Execute all the scripts.
    //
    ExecuteScripts( aParams, pScene->m_cScriptParameters, DeltaTime );

    //
    // Get the changed parameters back from the scripting engine.
    //
    for ( ScriptingScene::ObjectsIt it=pScene->m_Objects.begin(); it != pScene->m_Objects.end();
          it++ )
    {
        ScriptingObject* pObject = *it;
        
        //
        // Get the script parameters index for this object.
        //
        i32 iParams = pObject->m_iScriptParameters;

        //
        // If the position was changed get the new data.
        //
        if ( aParams[ iParams ].Changes & ScriptChanges::Position )
        {
            pObject->m_Position.x = aParams[ iParams ].Position[ 0 ];
            pObject->m_Position.y = aParams[ iParams ].Position[ 1 ];
            pObject->m_Position.z = aParams[ iParams ].Position[ 2 ];

            pObject->PostChanges( Interface::System::Changes::Geometry::Position );
        }

        //
        // If the orientation was changed get the new data.
        //
        if ( aParams[ iParams ].Changes & ScriptChanges::Orientation )
        {
            pObject->m_Orientation.x = aParams[ iParams ].Orientation[ 0 ];
            pObject->m_Orientation.y = aParams[ iParams ].Orientation[ 1 ];
            pObject->m_Orientation.z = aParams[ iParams ].Orientation[ 2 ];
            pObject->m_Orientation.w = aParams[ iParams ].Orientation[ 3 ];

            pObject->PostChanges( Interface::System::Changes::Geometry::Orientation );
        }

        //
        // If the scale was changed get the new data.
        //
        if ( aParams[ iParams ].Changes & ScriptChanges::Scale )
        {
            pObject->m_Scale.x = aParams[ iParams ].Scale[ 0 ];
            pObject->m_Scale.y = aParams[ iParams ].Scale[ 1 ];
            pObject->m_Scale.z = aParams[ iParams ].Scale[ 2 ];

            pObject->PostChanges( Interface::System::Changes::Geometry::Scale );
        }

        //
        // Clear out the changes for the next round.
        //
        aParams[ iParams ].Changes = ScriptChanges::None;

        iParams++;
    }
}
