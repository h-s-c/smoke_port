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



pcstr ScriptingObject::sm_kapszPropertyNames[] =
{
    "Script",
};

Properties::Property ScriptingObject::sm_kaDefaultProperties[ Property_Count ];


ScriptingObject::ScriptingObject(
    ISystemScene* pSystemScene
    )
    : ISystemObject( pSystemScene, NULL )
    , m_hScript( 0 )
{
    //
    // Some weirdness w/ the mixed clr doesn't allow this to be statically initialized.
    //
    sm_kaDefaultProperties[ 0 ] =
        Properties::Property( sm_kapszPropertyNames[ Property_Script ],
                              VALUE1( Properties::Values::String ),
                              Properties::Flags::Valid | Properties::Flags::InitOnly,
                              "File", NULL, NULL, NULL,
                              "" );

    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );
}


ScriptingObject::~ScriptingObject(
    void
    )
{
    UnloadScript( m_hScript );
}


Interface::System::Type
ScriptingObject::GetSystemType(
    void
    )
{
    return Interface::System::Types::Scripting;
}


Error
ScriptingObject::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        std::string sName = it->GetName();

        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            if ( sName == sm_kapszPropertyNames[ Property_Script ] )
            {
                ASSERTMSG( m_hScript == 0, "A script already exists for this object." );
                int ExpectedChanges;
                m_hScript = LoadScript( it->GetStringPtr( 0 ), ExpectedChanges );
                ASSERT( m_hScript != 0 );

                ScriptParameters& ScriptParameters =
                    reinterpret_cast<ScriptingScene*>(m_pSystemScene)->GetScriptParameters(
                        m_iScriptParameters
                        );
                ScriptParameters.HashCode = m_hScript;
                ScriptParameters.ExpectedChanges = ExpectedChanges;
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }

    //
    // Set the properties for this object.
    //
    SetProperties( Properties );

    return Errors::Success;
}


void
ScriptingObject::GetProperties(
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
ScriptingObject::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        std::string sName = it->GetName();

        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            //if ( sName == sm_kapszPropertyNames[ Property_GridSize ] )
            //{
            //}
            //else
            //{
                ASSERT( False );
            //}

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}


Interface::System::Types::BitMask
ScriptingObject::GetDesiredSystemChanges(
    void
    )
{
    return Interface::System::Changes::Geometry::All;
}


Error
ScriptingObject::ChangeOccurred(
    ISubject* pSubject,
    Interface::System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    //
    // Get the script parameters for this object.
    //
    ScriptParameters& ScriptParameters =
        reinterpret_cast<ScriptingScene*>(m_pSystemScene)->GetScriptParameters(
            m_iScriptParameters
            );

    if ( ChangeType & Interface::System::Changes::Geometry::Position )
    {
        m_Position = *dynamic_cast<IGeometryObject*>(pSubject)->GetPosition();

        if ( ScriptParameters.ExpectedChanges & ScriptChanges::Position )
        {
            ScriptParameters.Position[ 0 ] = m_Position.x;
            ScriptParameters.Position[ 1 ] = m_Position.y;
            ScriptParameters.Position[ 2 ] = m_Position.z;
            ScriptParameters.Changes |= ScriptChanges::Position;
        }
    }

    if ( ChangeType & Interface::System::Changes::Geometry::Orientation )
    {
        m_Orientation = *dynamic_cast<IGeometryObject*>(pSubject)->GetOrientation();

        if ( ScriptParameters.ExpectedChanges & ScriptChanges::Orientation )
        {
            ScriptParameters.Orientation[ 0 ] = m_Orientation.x;
            ScriptParameters.Orientation[ 1 ] = m_Orientation.y;
            ScriptParameters.Orientation[ 2 ] = m_Orientation.z;
            ScriptParameters.Orientation[ 2 ] = m_Orientation.w;
            ScriptParameters.Changes |= ScriptChanges::Orientation;
        }
    }

    if ( ChangeType & Interface::System::Changes::Geometry::Scale )
    {
        m_Scale = *dynamic_cast<IGeometryObject*>(pSubject)->GetScale();

        if ( ScriptParameters.ExpectedChanges & ScriptChanges::Scale )
        {
            ScriptParameters.Scale[ 0 ] = m_Scale.x;
            ScriptParameters.Scale[ 1 ] = m_Scale.y;
            ScriptParameters.Scale[ 2 ] = m_Scale.z;
            ScriptParameters.Changes |= ScriptChanges::Scale;
        }
    }

    return Errors::Success;
}


Interface::System::Changes::BitMask
ScriptingObject::GetPotentialSystemChanges(
    void
    )
{
    return Interface::System::Changes::Geometry::All;
}


const Base::Vector3*
ScriptingObject::GetPosition(
    void
    )
{
    return &m_Position;
}


const Base::Quaternion*
ScriptingObject::GetOrientation(
    void
    )
{
    return &m_Orientation;
}


const Base::Vector3*
ScriptingObject::GetScale(
    void
    )
{
    return &m_Scale;
}
