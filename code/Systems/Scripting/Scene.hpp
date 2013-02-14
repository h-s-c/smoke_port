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


class ScriptingSystem;
class ScriptingTask;
class ScriptingObject;


#pragma warning( push )
#pragma warning( disable : 4511 4512 )


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implements a scripting ISystemScene.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScriptingScene : ISystemScene
{
    friend ScriptingSystem;
    friend ScriptingTask;


protected:

    /// <summary>
    ///   Constructor.
    /// </summary>
    /// <param name="pSystem">The system this scene belongs to.</param>
    ScriptingScene( ISystem* pSystem );

    /// <summary>
    ///   Destructor.
    /// </summary>
    ~ScriptingScene( void );

    /// <summary cref="ISystemScene::GetSystemType">
    ///   Implementation of ISystemScene::GetSystemType.
    /// </summary>
    virtual Interface::System::Type GetSystemType( void );

    /// <summary cref="ISystemScene::Initialize">
    ///   Implementation of ISystemScene::Initialize.
    /// </summary>
    virtual Error Initialize( Properties::Array Properties );

    /// <summary cref="ISystemScene::GetProperties">
    ///   Implementation of ISystemScene::GetProperties.
    /// </summary>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="ISystemScene::SetProperties">
    ///   Implementation of ISystemScene::SetProperties.
    /// </summary>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="ISystemScene::GetObjectTypes">
    ///   Implementation of ISystemScene::GetObjectTypes.
    /// </summary>
    virtual pcstr* GetObjectTypes( void );

    /// <summary cref="ISystemScene::CreateObject">
    ///   Implementation of ISystemScene::CreateObject.
    /// </summary>
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );

    /// <summary cref="ISystemScene::DestroyObject">
    ///   Implementation of ISystemScene::DestroyObject.
    /// </summary>
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    /// <summary cref="ISystemScene::GetSystemTask">
    ///   Implementation of ISystemScene::GetSystemTask.
    /// </summary>
    virtual ISystemTask* GetSystemTask( void );

    /// <summary cref="ISubject::GetPotentialSystemChanges">
    ///   Implementation of ISubject::GetPotentialSystemChanges.
    /// </summary>
    virtual Interface::System::Changes::BitMask GetPotentialSystemChanges( void );


public:

    ScriptParameters& GetScriptParameters( i32 iScriptParameters )
    {
        ASSERT( iScriptParameters < m_cScriptParameters );
        return m_aScriptParameters[ iScriptParameters ];
    }


protected:

    ScriptingTask*                          m_pTask;

    typedef std::list<ScriptingObject*>     Objects;
    typedef Objects::iterator               ObjectsIt;

    Objects                                 m_Objects;

    ScriptParameters*                       m_aScriptParameters;
    i32                                     m_cScriptParameters;
    i32                                     m_cScriptParametersReserve;
    std::vector<i32>                        m_aEmptyScriptSlots;
};


#pragma warning( pop )
