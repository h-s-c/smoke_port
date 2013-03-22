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


class ScriptingScene;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implements a scripting ISystemTask.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScriptingTask : public ISystemTask
{
    friend class ScriptingScene;


protected:

    /// <summary>
    ///   Constructor.
    /// </summary>
    /// <param name="pScene">The scene this object belongs to.</param>
    ScriptingTask( ScriptingScene* pScene );

    /// <summary>
    ///   Destructor.
    /// </summary>
    ~ScriptingTask( void );

    /// <summary cref="ISystemTask::GetSystemType">
    ///   Implementation of ISystemTask::GetSystemType.
    /// </summary>
    virtual Interface::System::Type GetSystemType( void );

    /// <summary cref="ISystemTask::GetSystemType">
    ///   Implementation of ISystemTask::GetSystemType.
    /// </summary>
    virtual void Update( f32 DeltaTime );

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) { return true; } 
};
