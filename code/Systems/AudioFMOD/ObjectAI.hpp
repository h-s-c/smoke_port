#pragma once

//internal
#include "Systems/AudioFMOD/Object.hpp"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>FMODObjectAI</c> Implementation of AI sounds
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class FMODObjectAI : public FMODObject
{
public:
    FMODObjectAI( ISystemScene* pSystemScene );
    virtual ~FMODObjectAI( void );

protected:
    /// <summary cref="FMODObjectAI::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function for AI sounds.
    /// </summary>
    /// <param name="Properties">Initializes the sound object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="FMODObjectAI::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the sound object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="FMODObjectAI::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the sound object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="FMODObjectAI::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the sound object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="FMODObjectAI::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this sound.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="FMODObjectAI::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary >
    /// FMODObjectAI PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_SoundFile, Property_Count
    };
    
    /// <summary >
    /// FMODObjectAI Property name array
    /// </summary >
    static pcstr                        sm_kapszPropertyNames[];
    
    /// <summary >
    /// FMODObjectAI Property array of default values
    /// </summary >
    static const Properties::Property   sm_kaDefaultProperties[];

private:

    u32     m_AIState;  // State AI is in

    // Properties
    struct BehaviorAttributes
    {
        char*   m_soundFileName;
        int     m_loopingMode;
    };

    BehaviorAttributes m_BhvAttrs[e_Max_Behavior];
};

