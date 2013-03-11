#pragma once

//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
//
// forward declarations
//
class Bot;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Goal</c> Implementation of the base AI goal.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Goal
{
public:
    Goal( Bot* pBot );
    virtual ~Goal( void );

    /// <summary cref="Goal::GetName">
    ///   Gets the name for this goal.
    /// </summary>
    /// <returns>pcstr - Name of this goal.</returns>
    virtual pcstr GetName( void ) { return "Base Goal"; }

    /// <summary cref="Goal::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Goal::Update">
    /// This method is called each frame to update this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Goal::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PostUpdate( f32 DeltaTime );

protected:
    Bot* m_Bot;  // Pointer to the bot that owns this goal
};

