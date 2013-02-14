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


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implements a scripting ISystem.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScriptingSystem : public ISystem
{
public:

    /// <summary>
    ///   Constructor.
    /// </summary>
    ScriptingSystem( void );

    /// <summary>
    ///   Destructor.
    /// </summary>
    virtual ~ScriptingSystem( void );


protected:

    /// <summary cref="ISystem::GetName">
    ///   Implementation of ISystem::GetName.
    /// </summary>
    virtual pcstr GetName( void );

    /// <summary cref="ISystem::GetSystemType">
    ///   Implementation of ISystem::GetSystemType.
    /// </summary>
    virtual Interface::System::Type GetSystemType( void );

    /// <summary cref="ISystem::Initialize">
    ///   Implementation of ISystem::Initialize.
    /// </summary>
    virtual Error Initialize( Properties::Array Properties );

    /// <summary cref="ISystem::GetProperties">
    ///   Implementation of ISystem::GetProperties.
    /// </summary>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="ISystem::SetProperties">
    ///   Implementation of ISystem::SetProperties.
    /// </summary>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="ISystem::CreateScene">
    ///   Implementation of ISystem::CreateScene.
    /// </summary>
    virtual ISystemScene* CreateScene( void );

    /// <summary cref="ISystem::DestroyScene">
    ///   Implementation of ISystem::DestroyScene.
    /// </summary>
    virtual Error DestroyScene( ISystemScene* pSystemScene );


protected:

    enum PropertyTypes
    {
        Property_ScriptsLocation,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static Properties::Property         sm_kaDefaultProperties[];
};
