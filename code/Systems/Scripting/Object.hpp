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
class ScriptingTask;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implements a scripting ISystemObject and modifies geometry information.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScriptingObject : public ISystemObject, public IGeometryObject
{
    friend class ScriptingScene;
    friend class ScriptingTask;


protected:

    /// <summary>
    ///   Constructor.
    /// </summary>
    /// <param name="pSystemScene">The scene this object belongs to.</param>
    ScriptingObject( ISystemScene* pSystemScene );

    /// <summary>
    ///   Destructor.
    /// </summary>
    virtual ~ScriptingObject( void );

    /// <summary cref="ISystemObject::GetSystemType">
    ///   Implementation of ISystemObject::GetSystemType.
    /// </summary>
    virtual Interface::System::Type GetSystemType( void );

    /// <summary cref="ISystemObject::Initialize">
    ///   Implementation of ISystemObject::Initialize.
    /// </summary>
    virtual Error Initialize( Properties::Array Properties );

    /// <summary cref="ISystemObject::GetProperties">
    ///   Implementation of ISystemObject::GetProperties.
    /// </summary>
    void GetProperties( Properties::Array& Properties );

    /// <summary cref="ISystemObject::SetProperties">
    ///   Implementation of ISystemObject::SetProperties.
    /// </summary>
    void SetProperties( Properties::Array Properties );

    /// <summary cref="ISystemObject::GetDesiredSystemChanges">
    ///   Implementation of ISystemObject::GetDesiredSystemChanges.
    /// </summary>
    virtual Interface::System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="IObserver::ChangeOccurred">
    ///   Implementation of the IObserver ChangeOccurred function.
    /// </summary>
    virtual Error ChangeOccurred( ISubject* pSubject,
                                  Interface::System::Changes::BitMask ChangeType );

    /// <summary cref="ISubject::GetPotentialSystemChanges">
    ///   Implementation of the ISubject GetPotentialSystemChanges function.
    /// </summary>
    virtual Interface::System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="IGeometryObject::GetPosition">
    ///   Implementation of the IGeometryObject GetPosition function.
    /// </summary>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="IGeometryObject::GetOrientation">
    ///   Implementation of the IGeometryObject GetOrientation function.
    /// </summary>
    virtual const Base::Quaternion* GetOrientation( void );

    /// <summary cref="IGeometryObject::GetScale">
    ///   Implementation of the IGeometryObject GetScale function.
    /// </summary>
    virtual const Base::Vector3* GetScale( void );


protected:

    enum PropertyTypes
    {
        Property_Script,
        Property_Count
    };

    static pcstr                            sm_kapszPropertyNames[];
    static Properties::Property             sm_kaDefaultProperties[];

    i32                                     m_hScript;
    i32                                     m_iScriptParameters;

    Base::Vector3                           m_Position;
    Base::Quaternion                        m_Orientation;
    Base::Vector3                           m_Scale;
};
