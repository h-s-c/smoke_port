//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/ObjectAI.hpp"
#include "Systems/AudioFMOD/System.hpp"
//stdlib
#include <algorithm>
#include <cstring>
#include <mutex>
//external
#include "fmod.hpp"


pcstr FMODObjectAI::sm_kapszPropertyNames[] =
{
    "SoundFile",
};

const Properties::Property FMODObjectAI::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_SoundFile ],
                          VALUE4( Properties::Values::String, Properties::Values::String, 
                          Properties::Values::String, Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "", "", "", "" ),
};

///////////////////////////////////////////////////////////////////////////////
// FMODObjectAI - Constructor
FMODObjectAI::FMODObjectAI(
    ISystemScene* pSystemScene
    )
    : FMODObject( pSystemScene )
    , m_AIState ( 0 )
{
    // Compiler will optimize the loop out
    for ( size_t i = 0; i < e_Max_Behavior; ++i )
    {
        m_BhvAttrs[i].m_soundFileName = NULL;
        m_BhvAttrs[i].m_loopingMode = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// ~FMODObjectAI - Destructor
FMODObjectAI::~FMODObjectAI(
    void
    )
{
    for ( size_t i = 0; i < e_Max_Behavior; ++i )
    {
        delete m_BhvAttrs[i].m_soundFileName;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Object with the given properties
Error
FMODObjectAI::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Set this set as initialized.
    //
    FMODObject::Initialize( Properties );
    
    if ( m_pSystem->AudioInitialized() ) 
    {
        FMOD_VECTOR SourcePos = { m_Position[0], m_Position[1], m_Position[2] };
        FMOD_VECTOR SourceVel = { 0.0, 0.0, 0.0 };

        // Set initial state at Idle
        LoadSound( m_BhvAttrs[e_Behavior_Idle].m_soundFileName, &m_Buffer );
        m_Result = m_Buffer->setLoopCount( FMOD_LOOP_FOREVER * m_BhvAttrs[e_Behavior_Idle].m_loopingMode );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ ); 

        // Generate source/channel with sound paused
        m_Result = m_pSystem->m_pFMOD->playSound(FMOD_CHANNEL_FREE, m_Buffer, true, &m_Source);
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = m_Source->setChannelGroup( FMODSystem::statePlaying );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = m_Source->set3DAttributes( &SourcePos, &SourceVel );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = m_Source->setPriority( m_iPriority );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = m_Source->setVolume( m_fVolume );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = m_Source->setPaused( false );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
    } // end if ( m_pSystem->AudioInitialized )

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Object are returned in Properties
void
FMODObjectAI::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Call base class.
    //
    FMODObject::GetProperties( Properties );

    ////
    //// Add all the properties.
    ////
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }
}

///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Object
void
FMODObjectAI::SetProperties(
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
            if ( sName == sm_kapszPropertyNames[ Property_SoundFile ] )
            {
                // Set associated behavior
                std::string BehaviorName = it->GetString( 1 );
                Behavior bhv = e_Behavior_None;
                if( BehaviorName == "Idle" || BehaviorName == "idle" )
                {
                    bhv = e_Behavior_Idle;
                }
                else if( BehaviorName == "Walking" || BehaviorName == "walking" )
                {
                    bhv = e_Behavior_Walking;
                }
                else if( BehaviorName == "Running" || BehaviorName == "running" )
                {
                    bhv = e_Behavior_Running;
                }
                else if( BehaviorName == "Flying" || BehaviorName == "flying" )
                {
                    bhv = e_Behavior_Flying;
                }
                else if( BehaviorName == "Panicked" || BehaviorName == "panicked" )
                {
                    bhv = e_Behavior_Panicked;
                }

                if ( bhv != e_Behavior_None )
                {
                    pcstr   name = it->GetStringPtr( 0 );
                    size_t  len = std::min((size_t)FILENAME_MAX, strlen(name)) + 1;
                    m_BhvAttrs[bhv].m_soundFileName = new char[len];
                    strcpy_s( m_BhvAttrs[bhv].m_soundFileName, len, name );
                    // Set associated looping behavior
                    std::string lsFlag = it->GetString( 2 );
                    if( lsFlag == "Looping" || lsFlag == "looping" )
                    {
                        m_BhvAttrs[bhv].m_loopingMode = 1;
                    }
                    else
                    {
                        ASSERT (lsFlag == "Single" || lsFlag == "single");
                    }
                }

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }
    
    //
    // Let the base class process the properties.
    //
    FMODObject::SetProperties( Properties );
}

///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns the systems changes this Object 
//                           is intrested in hearing about
System::Changes::BitMask
FMODObjectAI::GetDesiredSystemChanges(
    void
    )
{
    return ( FMODObject::GetDesiredSystemChanges() | System::Changes::AI::Behavior );
}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)

Error
FMODObjectAI::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    if ( m_pSystem->AudioInitialized() ) 
    {
        if ( ChangeType & System::Changes::AI::Behavior )
        {
            IBehaviorObject* pBehaviorObject = dynamic_cast<IBehaviorObject*>(pSubject);
            Behavior bhv = pBehaviorObject->GetBehavior();
            
            // AI State changed, load the appropriate sound to reflect the AI state
            // and play it
            FMODObject::LoadSound( m_BhvAttrs[bhv].m_soundFileName, &m_Buffer );
            std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
            m_Result = m_Buffer->setLoopCount( FMOD_LOOP_FOREVER * m_BhvAttrs[bhv].m_loopingMode );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            // update source w/ right sound
            //m_Source->stop();
            m_Result = m_pSystem->m_pFMOD->playSound(FMOD_CHANNEL_REUSE, m_Buffer, true, &m_Source);
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setChannelGroup( FMODSystem::statePlaying );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setPriority( m_iPriority );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setVolume( m_fVolume );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setPaused( false );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        }
    }

    // Pass changes on to base class
    FMODObject::ChangeOccurred( pSubject, ChangeType );

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Object
System::Changes::BitMask
FMODObjectAI::GetPotentialSystemChanges(
    void
    )
{
    return FMODObject::GetPotentialSystemChanges();
}
