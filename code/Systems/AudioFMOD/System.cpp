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

//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/Object.hpp"
#include "Systems/AudioFMOD/Scene.hpp"
#include "Systems/AudioFMOD/System.hpp"
//stdlib
#include <vector>
#include <string>
//external
#include "fmod.hpp"

FMOD::ChannelGroup* FMODSystem::music;          // Holds sources that play music
FMOD::ChannelGroup* FMODSystem::statePlaying;   // Holds sources that are in a playing state
FMOD::ChannelGroup* FMODSystem::stateStopped;   // Holds sources that are in a non-playing state
FMOD::ChannelGroup* FMODSystem::stateStartup;   // Holds sources that will play at startup

pcstr FMODSystem::sm_kapszPropertyNames[] =
{
    "ResourceLocation",
    "MusicVol", "FXVol", "SpeakerMode",
};

const Properties::Property FMODSystem::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_ResourceLocation ],
                          VALUE1x3( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly |
                            Properties::Flags::Multiple | Properties::Flags::WriteOnly,
                          NULL, NULL, NULL, NULL,
                          "", "", "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_MusicVol ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_FXVol ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_SpeakerMode ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
};

///////////////////////////////////////////////////////////////////////////////
// FMODSystem - Constructor
FMODSystem::FMODSystem(
    void
    )
    : ISystem()
    , m_pFMOD ( NULL )
    , m_SpeakerMode ( 2 )
    , m_CPUStat ( 0.0f )
{
}

///////////////////////////////////////////////////////////////////////////////
// FMODSystem - Destructor
FMODSystem::~FMODSystem(
    void
    )
{
    // Close FMOD System
    m_pFMOD->release();
}

///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr
FMODSystem::GetName(
    void
    )
{
    return System::Names::Audio;
}

///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type
FMODSystem::GetSystemType(
    void
    )
{
    return System::Types::Audio;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error
FMODSystem::Initialize(
    Properties::Array Properties
    )
{

    //
    // Set properties passed in
    //
    SetProperties( Properties );
    
    // Assume successful FMOD initialization unless proven otherwise
    Error Err = Errors::Success;
    m_bAudioInitialized = true;
    
    u32              version;
    FMOD_SPEAKERMODE speakermode, smCaps;
    FMOD_CAPS        caps;

    switch(m_SpeakerMode)
    {
        // No speakers specified, so jump out of audio initialization
        case 0:
            //Debug::Print("No speakers specified... Disabling audio...\n");
            m_bAudioInitialized = false;
            return Err;
        case 1:
            speakermode = FMOD_SPEAKERMODE_MONO;
            break;
        case 2:
            speakermode = FMOD_SPEAKERMODE_STEREO;
            break;
        case 3:
            speakermode = FMOD_SPEAKERMODE_SURROUND;
            break;
        case 4:
            speakermode = FMOD_SPEAKERMODE_5POINT1;
            break;
        case 5:
            speakermode = FMOD_SPEAKERMODE_7POINT1;
            break;
        default:
            speakermode = FMOD_SPEAKERMODE_STEREO;
            break;
    }

    //
    // The following code is the recommended method to start up FMOD for Win32 applications.
    // This code was taken from the FMOD SDK and slightly modified for use
    //

    /*
        Create a System object and initialize.
    */
    m_Result = FMOD::System_Create( &m_pFMOD );   // Startup FMOD.
    if (m_Result != FMOD_OK)
    {
        Err = Errors::Failure;
        m_bAudioInitialized = false;
        return Err;
    }

    m_Result = m_pFMOD->getVersion(&version);
    FMODErrorCode( m_Result, __FUNCTION__ );

    if (version < FMOD_VERSION)
    {
        Err = Errors::Failure;
        //Debug::Print("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        m_bAudioInitialized = false;
        return Err;
    }
    
    m_Result = m_pFMOD->getDriverCaps(0, &caps, 0, &smCaps);
    if (m_Result != FMOD_OK)
    {
        Err = Errors::Failure;
        //Debug::Print("No sound device detected... disabling audio\n");
        m_bAudioInitialized = false;
        return Err;
    }

    // Check if sound card supports specified speaker mode
    if ( smCaps < speakermode )
    {
        Err = Errors::Failure;
        //Debug::Print("Warning!  The speaker mode selected ($d) is not supported. \n", m_SpeakerMode);
        m_bAudioInitialized = false;
        return Err;
    }

    m_Result = m_pFMOD->setSpeakerMode( speakermode );
    FMODErrorCode( m_Result, __FUNCTION__ );

    if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
    {                                                   /* You might want to warn the user about this. */
        m_Result = m_pFMOD->setDSPBufferSize(1024, 10);    /* At 48khz, the latency between issuing an fmod command and hearing it will now be about 213ms. */
    }

    m_Result = m_pFMOD->init(FMOD_NUM_SOURCES, FMOD_INIT_3D_RIGHTHANDED, 0);
    FMODErrorCode( m_Result, __FUNCTION__ );
    
    if (m_Result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        m_Result = m_pFMOD->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        m_Result = m_pFMOD->init(1000, FMOD_INIT_NORMAL, 0); /* Replace with whatever channel count and flags you use! */
    }
    
    // 100.0f used to scale from meter to cm
    m_Result = m_pFMOD->set3DSettings( 1.0f, 100.0f, 1.0f); 
    FMODErrorCode( m_Result, __FUNCTION__ );

    //
    // Create Channel groups
    //
    m_Result = m_pFMOD->createChannelGroup( "music",   &music );
    FMODErrorCode( m_Result, __FUNCTION__ );
    m_Result = m_pFMOD->createChannelGroup( "playing", &statePlaying );
    FMODErrorCode( m_Result, __FUNCTION__ );
    m_Result = m_pFMOD->createChannelGroup( "stopped", &stateStopped );
    FMODErrorCode( m_Result, __FUNCTION__ );
    m_Result = m_pFMOD->createChannelGroup( "startup", &stateStartup );

    m_bInitialized = True;

    return Err;
}

///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void
FMODSystem::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }

    if ( m_bInitialized )
    {
        Properties[ iProperty+Property_MusicVol ].SetValue( 0, 0 );   // get value from FMOD
        Properties[ iProperty+Property_FXVol ].SetValue( 0, 0 );   // get value from FMOD
    }
}

///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void
FMODSystem::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_ResourceLocation ] )
            {
                szSoundDirName = it->GetStringPtr( 0 );

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_MusicVol ] )
            {
                // call FMOD to set MusicVol
                // to be implemented later
            }
            else if ( sName == sm_kapszPropertyNames[ Property_FXVol ] )
            {
                // call FMOD to set FXVol
                // to be implemented later
            }
            else if ( sName == sm_kapszPropertyNames[ Property_SpeakerMode ] )
            {
                m_SpeakerMode = it->GetInt32( 0 );

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene*
FMODSystem::CreateScene(
    void
    )
{
    return new FMODScene( this );
}

///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destorys the given Scene, free all associated resources
Error
FMODSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    FMODScene* pScene = reinterpret_cast<FMODScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// SetCPUUsage - Sets CPU Utilization
void 
FMODSystem::SetCPUUsage( 
    f32 CPU 
    )
{
    m_CPUStat = CPU;
}
    
///////////////////////////////////////////////////////////////////////////////
// GetCPUUsage - Gets CPU Utilization
f32 
FMODSystem::GetCPUUsage( 
    void 
    )
{
    return m_CPUStat;
}

///////////////////////////////////////////////////////////////////////////////
// PauseAll - Pause all sounds
void 
FMODSystem::PauseAll( 
    void 
    ) 
{
    music->setPaused( true );
    statePlaying->setPaused( true );
}

///////////////////////////////////////////////////////////////////////////////
// PlayAll - Plays all sounds by unpausing them
void 
FMODSystem::PlayAll( 
    void 
    ) 
{
    music->setPaused( false );
    statePlaying->setPaused( false );
}

///////////////////////////////////////////////////////////////////////////////
// ResumeAll - Resumes all sounds that were playing by unpausing them
void 
FMODSystem::ResumeAll( 
    void 
    ) 
{
    music->setPaused( false );
    statePlaying->setPaused( false );
}

///////////////////////////////////////////////////////////////////////////////
// MuteMusic - Mutes background music only by pausing it
void FMODSystem::MuteMusic( 
    void 
    ) 
{
    music->setPaused( true );
}

///////////////////////////////////////////////////////////////////////////////
// UnmuteMusic - Resumes background music only by unpausing it
void FMODSystem::UnmuteMusic( 
    void 
    ) 
{
    music->setPaused( false );
}

///////////////////////////////////////////////////////////////////////////////
// AudioInitialized - Is the audio initialized?
bool FMODSystem::AudioInitialized( 
    void 
    ) 
{
    return m_bAudioInitialized;
}


///////////////////////////////////////////////////////////////////////////////
// FMODErrorCode - Print the FMOD error code
void FMODSystem::FMODErrorCode(
    i32 FMODresult,
    const char* file
    )
{
    if ( FMODresult != FMOD_OK )
    {
        //Debug::Print("Error %d called from %s\n", FMODresult, file);
    }
}
