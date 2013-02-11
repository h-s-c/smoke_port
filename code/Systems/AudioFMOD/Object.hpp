#pragma once

//stdlib
#include <ctime>
//external
#include "fmod.hpp"

//forward declarations
class FMODSystem;
class FMODScene;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>FMODObject</c> Implementation of the ISystemObject interface.  
///   This is the base object created by the FMOD Scene.  
///   One object is created per sound.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class FMODObject : public ISystemObject
{
    friend FMODSystem;
    friend FMODScene;

protected:

    FMODObject( ISystemScene* pSystemScene );
    virtual ~FMODObject( void );

    /// <summary cref="FMODObject::GetSystemType">
    ///   Implementation of the <c>ISystemObject::GetSystemType</c> function.
    ///   Lets this object know when a registered aspects of interest has changed 
    ///   (this function will be called when other systems make changes this object should know about).
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="FMODObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the sound object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="FMODObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the sound object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="FMODObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the sound object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="FMODObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the sound object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="FMODObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this sound.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="FMODObject::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="FMODObject::LoadSound">
    ///   Takes a filename and creates a new sound object or provides the address
    ///   of a sound object if it has already been created
    /// </summary>
    /// <param name="soundFileName">Filename of sound.</param>
    /// <param name="s">Sound object.</param>
    /// <returns>Error.</returns>
    Error LoadSound( char* soundFileName, FMOD::Sound** s );

    /// <summary cref="FMODObject::UpdateListener">
    ///   Updates the listeners position and orientation
    /// </summary>
    /// <param name="CurrPos">Position of listener to update to</param>
    /// <param name="CurrOri">Orientation of listener to update to</param>
    /// <returns>Error.</returns>
    Error UpdateListener( Base::Vector3 *CurrPos, Base::Quaternion *CurrOri );
    
    /// <summary cref="FMODObject::UpdatePosition">
    ///   Updates the position for any source
    /// </summary>
    /// <param name="CurrPos">Position of the source to update to</param>
    /// <returns>Error.</returns>
    Error UpdatePosition( Base::Vector3 *CurrPos );  // Handle changes in position
    
    /// <summary cref="FMODObject::RandomPlay">
    ///   Play the sound specified as random upon initialization at a random time
    /// </summary>
    void  RandomPlay(void);


protected:

    /// <summary >
    /// FMODObject PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_SoundFile, Property_Listener, Property_Source,
        Property_Looping, Property_Random, Property_Intermittent, Property_Fire, 
        Property_Water, Property_Position, Property_Orientation, Property_Volume, 
        Property_Priority, Property_Count
    };

    /// <summary >
    /// FMODObject Property name array
    /// </summary >
    static pcstr                        sm_kapszPropertyNames[];
    
    /// <summary >
    /// FMODObject Property array of default values
    /// </summary >
    static const Properties::Property   sm_kaDefaultProperties[];

    FMODSystem* m_pSystem;  // FMOD System pointer
    FMOD_RESULT m_Result;   // FMOD Error

    // Properties set upon initialization
    Bool            m_bListener;
    Bool            m_bSource;
    Bool            m_bLooping;
    Bool            m_bRandom;
    Bool            m_bIntermittent;
    Bool            m_bFire;
    Bool            m_bWater;
    Bool            m_bCollisionOff;
    f32             m_fVolume;
    u32             m_iPriority;
    char            m_szSoundFileName[FILENAME_MAX];

    FMOD::Sound*    m_Buffer; // holds the sound
    FMOD::Channel*  m_Source; // where the sound plays

    Base::Vector3       m_Position;     // Position of sound object
    Base::Quaternion    m_Orientation;  // Orientation of sound object
    
    static Base::Vector3 prevPos;       // previous position for velocity calculation
    
    Bool            m_FireActive; // when fire active, unpause fire sounds

    // time variables for random play
    static clock_t       prevTime;
    u32                  timeout;
    u32                  endtime;
    
    // looping constants
    static const i32 FMOD_LOOP_FOREVER = -1;
    static const i32 FMOD_NO_LOOP      =  0;
};

