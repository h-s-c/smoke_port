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

#include "Systems/Ai/Bots/Bot.hpp"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Animal</c> Implementation of the base animal AI.  This class implements
///   functionality common to all animals.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Animal : public Bot
{
public:
    Animal( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~Animal( void );

    /// <summary cref="Animal::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Bot::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Animal::Update">
    /// This method is called each frame to update this Animal AI.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Bot::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Animal::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Bot::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

    /// <summary cref="Animal::UpdateFear">
    /// This method is called each frame to update fear parameters for this Animal.  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void UpdateFear( f32 DeltaTime );

    f32 m_Fear;                        // The level of fear for this animal
    f32 m_PanicLevel;                  // The level of fear that makes us panic
    Base::Vector3 m_FearVector;        // Vector pointing in least scary direction

    Bool m_CanFly;                     // Can this creature fly?
};

