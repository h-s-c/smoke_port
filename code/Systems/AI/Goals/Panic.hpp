#pragma once

#include "Systems/AI/Goals/Goal.hpp"


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Panic</c> Implementation of a panic goal.  This goal will result in 
///   running away from something that has caused fear.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Panic : public Goal
{
public:
    Panic( Bot* pBot );
    virtual ~Panic( void );

    /// <summary cref="Panic::GetName">
    ///   Gets the name for this goal ("Panic").
    /// </summary>
    /// <returns>pcstr - Name of this goal.</returns>
    /// <seealso cref="Goal::GetName"/>
    virtual pcstr GetName( void ) { return "Panic"; }

    /// <summary cref="Panic::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Panic::Update">
    /// This method is called each frame to update this goal.  
    /// Accelrate and move the AI away from m_FearVector.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Panic::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );
};

