#pragma once

#include "Systems/AI/Bots/Animal.hpp"


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Swallow</c> Implementation of the swallow AI. 
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Swallow : public Animal
{
public:
    Swallow( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~Swallow( void );

    
    /// <summary cref="Swallow::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Swallow::Update">
    /// This method is called each frame to update this swallow AI.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Swallow::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

protected:
    /// <summary cref="Swallow::UpdateFlock">
    /// This method is call every frame when the swallow is flocking.
    /// This function does all updates specifically required for flocking.
    /// </summary>
    void UpdateFlock( void );

    /// <summary cref="Swallow::UpdatePanic">
    /// This method is call every frame when the swallow is panicking.
    /// This function does all updates specifically required for panicking.
    /// </summary>
    void UpdatePanic( void );

    Math::Vector3 m_TargetPosition;
};

