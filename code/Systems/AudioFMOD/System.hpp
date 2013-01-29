#pragma once

//stdlib
#include <mutex>

// maximum number of sources supported
// change according to hardware specs of sound card
#define MAX_SOURCES 24;

// Forward declare
namespace FMOD
{
    class System;
    class ChannelGroup;
}


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>FMODSystem</c> Implementation of the ISystem interface for generic sound 
///   functionality.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class FMODSystem : public ISystem
{

public:

    FMODSystem( void );
    virtual ~FMODSystem( void );
    
    /// <summary cref="FMODSystem::SetCPUUsage">
    ///   Sets the CPU Utilization of FMOD
    /// </summary>
    /// <param name="CPU">CPU Utilization (0-100f)</param>
    void SetCPUUsage( f32 CPU );
    
    /// <summary cref="FMODSystem::GetCPUUsage">
    ///   Returns the CPU Utilization of FMOD
    /// </summary>
    /// <returns>CPU Utilization (0-100f)</returns>
    virtual f32 GetCPUUsage( void );

    /// <summary cref="FMODSystem::PauseAll">
    ///   Pause all sounds
    /// </summary>
    static void PauseAll( void );

    /// <summary cref="FMODSystem::PlayAll">
    ///   Plays all sounds by unpausing them
    /// </summary>
    static void PlayAll( void );

    /// <summary cref="FMODSystem::ResumeAll">
    ///   Resumes all sounds that were playing by unpausing them
    /// </summary>
    static void ResumeAll( void );

    /// <summary cref="FMODSystem::MuteMusic">
    ///   Mutes background music only by pausing it
    /// </summary>
    static void MuteMusic( void );
 
    /// <summary cref="FMODSystem::UnmuteMusic">
    ///   Resumes background music only by unpausing it
    /// </summary>
    static void UnmuteMusic( void );

    /// <summary cref="FMODSystem::AudioInitialized">
    ///   Resumes background music only by unpausing it
    /// </summary>
    /// <returns>Audio initilzation state</returns>
    bool AudioInitialized( void );

    /// <summary cref="FMODSystem::FMODErrorCode">
    ///   Prints the error code returned by FMOD in a particular file (in Debug)
    /// </summary>
    /// <param name="FMODresult">FMOD error code generated</param>
    /// <param name="file">The file where the FMOD error code was generated</param>
    void FMODErrorCode( i32 FMODresult, const char* file );

    pstr         ResourceLoc[256];              // Resource location for sounds
    std::string  szSoundDirName;                // Directory to sound resources
    FMOD::System *m_pFMOD;                      // Pointer to FMOD

    static FMOD::ChannelGroup* music;           // Holds sources that play music
    static FMOD::ChannelGroup* statePlaying;    // Holds sources that are in a playing state
    static FMOD::ChannelGroup* stateStopped;    // Holds sources that are in a non-playing state
    static FMOD::ChannelGroup* stateStartup;    // Holds sources that will play at startup

    std::mutex m_mutex;

protected:

    /// <summary cref="FMODSystem::GetName">
    ///   Implementation of the <c>ISystem::GetName</c> function.
    ///   Gets the name of the system.  Only custom systems can return a custom name.
    /// </summary>
    /// <returns>pcstr - The name of the system.</returns>
    /// <seealso cref="ISystem::GetName"/>
    virtual pcstr GetName( void );

    /// <summary cref="FMODSystem::GetSystemType">
    ///   Implementation of the <c>ISystem::GetSystemType</c> function.
    ///   Gets the system type for this system.
    /// </summary>
    /// <returns>System::Type - The type of the system.</returns>
    /// <seealso cref="ISystem::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="FMODSystem::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    ///   One time initialization function for the system.
    /// </summary>
    /// <param name="Properties">Property structure array to initialize.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( Properties::Array Properties );

    /// <summary cref="FMODSystem::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    ///   Gets the properties of this system.
    /// </summary>
    /// <param name="Properties">Property structure array to fill</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="FMODSystem::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    ///   Sets the properties for this system.
    /// </summary>
    /// <param name="Properties">Properties to set in the system.</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="FMODSystem::CreateScene">
    ///   Implementation of the <c>ISystem::CreateScene</c> function.
    ///   Creates a system scene for containing system objects.
    /// </summary>
    /// <returns>ISystemScene* - The newly create system scene.</returns>
    /// <seealso cref="ISystem::CreateScene"/>
    virtual ISystemScene* CreateScene( void );

    /// <summary cref="FMODSystem::DestroyScene">
    ///   Implementation of the <c>ISystem::DestroyScene</c> function.
    ///   Destroys a system scene.
    /// </summary>
    /// <param name="pSystemScene">The scene to destroy. Any objects within are destroyed.</param>
    /// <returns>Error - Any error codes.</returns>
    /// <seealso cref="ISystem::DestroyScene"/>
    virtual Error DestroyScene( ISystemScene* pSystemScene );

    /// <summary >
    /// FMODSystem PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_ResourceLocation,
        Property_MusicVol, 
        Property_FXVol, 
        Property_SpeakerMode,
        Property_Count
    };

    /// <summary >
    /// FMODSystem Property name array
    /// </summary >
    static pcstr                        sm_kapszPropertyNames[];
    
    /// <summary >
    /// FMODSystem Property array of default values
    /// </summary >
    static const Properties::Property   sm_kaDefaultProperties[];

    u32     m_Result;           // FMOD Error
    i32     m_SpeakerMode;      // Set with property
    f32     m_CPUStat;          // CPU Utilization
    bool    m_bAudioInitialized;// FMOD initialized
    
    static const i32 FMOD_NUM_SOURCES = 1000; // number of sources
};

