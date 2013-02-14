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
#include "Task.h"
#include "Object.h"


ScriptingScene::ScriptingScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )

{
    m_pTask = new ScriptingTask( this );

    m_aScriptParameters = new ScriptParameters[ 20 ];
    ASSERT( m_aScriptParameters != NULL );

    m_cScriptParameters = 0;
    m_cScriptParametersReserve = 20;
}


ScriptingScene::~ScriptingScene(
    void
    )
{
    //
    // Free all the remaining objects.
    //
    for ( std::list<ScriptingObject*>::iterator it=m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();

    UnloadAllScripts();

    SAFE_DELETE_ARRAY( m_aScriptParameters );
}


Interface::System::Type
ScriptingScene::GetSystemType(
    void
    )
{
    return Interface::System::Types::Scripting;
}


Error
ScriptingScene::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

	m_bInitialized = True;

    return Errors::Success;
}


void
ScriptingScene::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


void
ScriptingScene::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}

    
pcstr*
ScriptingScene::GetObjectTypes(
    void
    )
{
    return NULL;
}


ISystemObject*
ScriptingScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    UNREFERENCED_PARAM( pszName );
    UNREFERENCED_PARAM( pszType );

    ASSERT( m_bInitialized );
    ASSERT( pszType == NULL );

    //
    // Create the object and add it to the object list.
    //
    ScriptingObject* pObject = new ScriptingObject( this );

    if ( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    //
    // Add a script parameters entry for this object.
    //
    i32 iScriptParamters = -1;

    if ( m_aEmptyScriptSlots.size() > 0 )
    {
        //
        // If there are empty slots then use them.
        //
        iScriptParamters = m_aEmptyScriptSlots.back();
        m_aEmptyScriptSlots.pop_back();
    }
    else
    {
        //
        // Check if the buffer is big enough.
        //
        if ( m_cScriptParameters+1 > m_cScriptParametersReserve )
        {
            m_cScriptParametersReserve += 20;
            ScriptParameters* aScriptParameters = new ScriptParameters[ m_cScriptParametersReserve ];
            ASSERT( aScriptParameters != NULL );

            //
            // Copy over the information.
            //
            ::memcpy_s( aScriptParameters, m_cScriptParametersReserve * sizeof (ScriptParameters),
                        m_aScriptParameters, m_cScriptParameters * sizeof (ScriptParameters) );

            //
            // Delete the old array.
            //
            ScriptParameters* pTemp = m_aScriptParameters;
            m_aScriptParameters = aScriptParameters;
            delete [] pTemp;
        }

        iScriptParamters = m_cScriptParameters;
    }

    pObject->m_iScriptParameters = iScriptParamters;
    m_aScriptParameters[ m_cScriptParameters ].HashCode = 0;
    m_aScriptParameters[ m_cScriptParameters ].Changes = ScriptChanges::None;
    m_cScriptParameters++;

    return pObject;
}


Error
ScriptingScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a ScriptingObject so that the correct destructor will be called.
    //
    ScriptingObject* pObject = reinterpret_cast<ScriptingObject*>(pSystemObject);

    //
    // Free up the script slot for this object.
    //
    m_aEmptyScriptSlots.push_back( pObject->m_iScriptParameters );
    m_aScriptParameters[ pObject->m_iScriptParameters ].HashCode = 0;

    //
    // Remove the object from the list and delete it.
    //
    m_Objects.remove( pObject );

    SAFE_DELETE( pObject );

    return Errors::Success;
}


ISystemTask*
ScriptingScene::GetSystemTask(
    void
    )
{
    return m_pTask;
}


Interface::System::Changes::BitMask
ScriptingScene::GetPotentialSystemChanges(
    void
    )
{
    return Interface::System::Changes::None;
}
