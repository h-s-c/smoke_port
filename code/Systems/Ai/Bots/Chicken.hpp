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

#pragma once

#include "Systems/Ai/Bots/Animal.hpp"


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Chicken</c> Implementation of the chicken AI. 
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Chicken : public Animal
{
public:
    Chicken( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~Chicken( void );

    /// <summary cref="Chicken::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Chicken::Update">
    /// This method is called each frame to update this chicken AI.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Chicken::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Animal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

protected:
    /// <summary cref="Chicken::UpdateIdle">
    /// This method is call every frame when the chicken is idling.
    /// This function does all updates specifically required for ideling.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    void UpdateIdle( f32 DeltaTime );

    /// <summary cref="Chicken::UpdateFlock">
    /// This method is call every frame when the chicken is flocking.
    /// This function does all updates specifically required for flocking.
    /// </summary>
    void UpdateFlock( void );

    /// <summary cref="Chicken::UpdatePanic">
    /// This method is call every frame when the chicken is panicking.
    /// This function does all updates specifically required for panicking.
    /// </summary>
    void UpdatePanic( void );

    Base::Vector3 m_IdleDirection;    // The desired facing direction when idealing
    f32           m_CurrentMaxSpeed;  // Current max speed for this chicken
    f32           m_Duration;         // Time before next state change
     
};

