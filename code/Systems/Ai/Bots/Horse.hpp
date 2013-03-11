#pragma once

#include "Systems/Ai/Bots/Animal.hpp"


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Horse</c> Implementation of the horse AI. 
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Horse : public Animal
{
public:
    Horse( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~Horse( void );

    /// <summary cref="Horse::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Horse::Update">
    /// This method is called each frame to update this chicken AI.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Horse::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

    /// <summary cref="Horse::UpdateFear">
    /// This method is called each frame to update fear parameters for this Horse.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::UpdateFear"/>
    virtual void UpdateFear( f32 DeltaTime );

protected:
    /// <summary cref="Horse::UpdateIdle">
    /// This method is call every frame when the horse is idling.
    /// This function does all updates specifically required for ideling.
    /// </summary>
    void UpdateIdle( void );

    /// <summary cref="Horse::UpdateFlocking">
    /// This method is call every frame when the horse is flocking.
    /// This function does all updates specifically required for flocking.
    /// </summary>
    void UpdateFlocking( void );

    /// <summary cref="Horse::UpdateHerding">
    /// This method is call every frame when the horse is herding.
    /// This function does all updates specifically required for herding.
    /// </summary>
    void UpdateHerding( void );

    /// <summary cref="Horse::UpdatePanic">
    /// This method is call every frame when the horse is panicking.
    /// This function does all updates specifically required for panicking.
    /// </summary>
    void UpdatePanic( void );

    /// <summary cref="Horse::UpdateWander">
    /// This method is call every frame when the horse is wandering.
    /// This function does all updates specifically required for wandering.
    /// </summary>
    void UpdateWander( void );

    /// <summary cref="Horse::NearPanickedHorse">
    /// This returns true if this Horse is neat another Horse in the Panic state.
    /// </summary>
    /// <returns>Bool - Returns True if Horse is near another panicked horse</returns>
    Bool NearPanickedHorse( void );

    f32 m_Duration;   // Time before next state change 
    f32 m_WalkSpeed;  // Walking speed for this horse
    f32 m_RunSpeed;   // Running speed for this horse

    Base::Vector3 m_TargetPosition;
};

