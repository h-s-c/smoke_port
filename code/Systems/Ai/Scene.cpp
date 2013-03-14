// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Standard Library
#include <mutex>
// System
#include "Systems/Common/POI.hpp"
#include "Systems/Ai/Bots/Animal.hpp"
#include "Systems/Ai/Bots/Bot.hpp"
#include "Systems/Ai/Bots/Chicken.hpp"
#include "Systems/Ai/Bots/Horse.hpp"
#include "Systems/Ai/Bots/Swallow.hpp"
#include "Systems/Ai/Object.hpp"
#include "Systems/Ai/ObjectCamBot.hpp"
#include "Systems/Ai/Scene.hpp"
#include "Systems/Ai/Task.hpp"


extern ManagerInterfaces   g_Managers;

// Prototypes
void ProcessAI( void* Data );
u32 JobsComplete( void* pScene );

///////////////////////////////////////////////////////////////////////////////
// AIScene - Constructor
AIScene::AIScene( ISystem* pSystem ) : ISystemScene( pSystem ), m_pAITask( NULL ), m_bParallelize(True)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~AIScene - Destructor
AIScene::~AIScene( void )
{
    SAFE_DELETE( m_pAITask );

    // Free all the remaining objects.
    for( std::vector<AIObject*>::iterator it = m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();


    // Free all the remaining POI.
    for( std::list<POI*>::iterator it = m_POI.begin(); it != m_POI.end(); it++ )
    {
        delete *it;
    }

    m_POI.clear();
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Scene
System::Type AIScene::GetSystemType( void )
{
    return System::Types::AI;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Scene with the given properties
Error AIScene::Initialize( std::vector<Properties::Property> Properties )
{
    ASSERT( !m_bInitialized );

    // Check if threading is enabled
    m_bParallelize = g_Managers.pTask != NULL && 
        g_Managers.pEnvironment->Variables().GetAsBool( "AI::Parallel", True );

    // Create a new AITask
    m_pAITask = new AITask( this );
    ASSERT( m_pAITask != NULL );

    // Initialized
    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Scene are returned in Properties
void AIScene::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Scene
void AIScene::SetProperties( Properties::Array Properties )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// GetObjectTypes - Get Object types for this Scene
pcstr* AIScene::GetObjectTypes( void )
{
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// CreateObject - Create an Object with the given Name and Type
ISystemObject* AIScene::CreateObject( pcstr pszName, pcstr pszType )
{
    UNREFERENCED_PARAM( pszType );

    ASSERT( m_bInitialized );

    // Create the AI object
    AIObject* pObject = NULL;
    if( strcmp( pszType, "Bot" ) == 0 )
    {
        pObject = (AIObject*)new Bot( this, pszName );
    }
    else if( strcmp( pszType, "Animal" ) == 0 )
    {
        pObject = (AIObject*)new Animal( this, pszName );
    }
    else if( strcmp( pszType, "Chicken" ) == 0 )
    {
        pObject = (AIObject*)new Chicken( this, pszName );
    }
    else if( strcmp( pszType, "Horse" ) == 0 )
    {
        pObject = (AIObject*)new Horse( this, pszName );
    }
    else if( strcmp( pszType, "Swallow" ) == 0 )
    {
        pObject = (AIObject*)new Swallow( this, pszName );
    }
    else if( strcmp( pszType, "CamBot" ) == 0 )
    {
        pObject = (AIObject*)new CamBot( this, pszName );
    }
    else
    {
        // Create a default AI object (this is probably an error, so assert)
        ASSERT( false );
        pObject = new AIObject( this, pszName );
    }

    if( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    return pObject;
}


///////////////////////////////////////////////////////////////////////////////
// DestroyObject - Destorys the given Object, removing it from the Scene
Error AIScene::DestroyObject( ISystemObject* pSystemObject )
{
    ASSERT( m_bInitialized );
    ASSERT( pSystemObject != NULL );

    // Cast to a AIObject so that the correct destructor will be called.
    AIObject* pObject = reinterpret_cast<AIObject*>(pSystemObject);

    // Remove the object from the list and delete it
    u32 index = 0;
    for( std::vector<AIObject*>::iterator it=m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        if( *it == pObject )
        {
            m_Objects.erase( m_Objects.begin() + index );
            break;
        }
        index++;
    }

    SAFE_DELETE( pSystemObject );

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemTask - Returns the task associated with this Scene
ISystemTask* AIScene::GetSystemTask( void )
{
    return m_pAITask;
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Scene
System::Changes::BitMask AIScene::GetPotentialSystemChanges( void )
{
    return System::Changes::None;
}


///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns systems this Scene is interested in
System::Changes::BitMask AIScene::GetDesiredSystemChanges( void )
{
    return System::Changes::POI::Area | System::Changes::POI::Contact;
}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Process changes that occured by other systems/objects
Error AIScene::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    if( ChangeType & System::Changes::POI::Area )
    {
        IAreaObject* pAreaObject = dynamic_cast<IAreaObject*>(pSubject);
        ASSERT( pAreaObject );

        // Store this area so objects can process them later
        // (assuming all areas are fire)
        pcstr AreaName = pAreaObject->GetAreaName();

        POIFire* pFire = NULL;

        // Check if we already have a fire with the same name
        m_mutex.lock();
        for( std::list<POI*>::iterator it = m_POI.begin(); it != m_POI.end(); it++ )
        {
            POI* pPOI = *it;
            if( pPOI->GetType() == POIType::e_POI_Fire )
            {
                POIFire* pTestFire = (POIFire*)pPOI;
                if( strcmp( pTestFire->GetName().c_str(), AreaName ) == 0 )
                {
                    pFire = pTestFire;
                    break;
                }
            }
        }

        // If we didn't find an exisiting fire POI, create a new one
        if( !pFire )
        {
            pFire = new POIFire();
            m_POI.push_back( pFire );

            pFire->SetName( AreaName );
        }
        m_mutex.unlock();

        // Set data
        Base::Vector3 Min, Max;
        pAreaObject->GetAreaBB( Min, Max );
        pFire->SetAABB( Min, Max );

        Base::Vector3 Center = ( Min + Max ) * 0.5f;
        pFire->SetPosition( Center );
    }
    else if( ChangeType & System::Changes::POI::Contact )
    {
        IContactObject* pContactObject = dynamic_cast<IContactObject*>(pSubject);
        ASSERT( pContactObject );

        const IContactObject::Info* pContactInfo = pContactObject->GetContact();

        // Store the contact points so objects can process them later
        POIContact* pContact = new POIContact();
        pContact->SetPosition( pContactInfo->m_Position );
        pContact->SetImpact( pContactInfo->m_Impact );

        std::lock_guard<std::mutex> lock(m_mutex);
        m_POI.push_back( pContact );
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// UpdateCallback - Invoked by ParallelFor algorithm to process a range of objects
void AIScene::UpdateCallback( void *param, u32 begin, u32 end )
{
    AIScene *pThis = static_cast<AIScene*>(param);
    pThis->ProcessRange( begin, end );
}

///////////////////////////////////////////////////////////////////////////////
// ProcessRange - Processes a range of objects
void AIScene::ProcessRange ( u32 begin, u32 end )
{
    for ( size_t i = begin; i < end; ++i )
    {
        AIObject* pObject = m_Objects[i];

        pObject->PreUpdate( m_fDeltaTime );
        pObject->Update( m_fDeltaTime );
        pObject->PostUpdate( m_fDeltaTime );
    }
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main Update for the AI Scene
void AIScene::Update(f32 fDeltaTime)
{
    static const u32 GrainSize = 80;

    m_fDeltaTime = fDeltaTime;

    u32 uSize = (u32)m_Objects.size();

    if( m_bParallelize
     && g_Managers.pTask != NULL
     && GrainSize < uSize )
    {
        g_Managers.pTask->ParallelFor( m_pAITask, UpdateCallback, this, 0, uSize, GrainSize );
    }
    else
    {
        ProcessRange( 0, (u32)m_Objects.size() );
    }
    PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void AIScene::PostUpdate( void )
{
    // Loop through and remove and expired POI
    std::list<POI*>::iterator it = m_POI.begin();
    while( it != m_POI.end() )
    {
        POI* pPOI = *it;
        if( pPOI->Expired() )
        {
            it = m_POI.erase( it );
            delete pPOI;
        }
        else
        {
            it++;
        }
    }
}
