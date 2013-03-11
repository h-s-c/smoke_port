#pragma once

#include "Systems/Ai/Goals/Goal.hpp"


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Idle</c> Implementation of a simple idle goal.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Idle : public Goal
{
public:
    Idle( Bot* pBot );
    virtual ~Idle( void );

    /// <summary cref="Idle::GetName">
    ///   Gets the name for this goal ("Idle").
    /// </summary>
    /// <returns>pcstr - Name of this goal.</returns>
    /// <seealso cref="Goal::GetName"/>
    virtual pcstr GetName( void ) { return "Idle"; }

    /// <summary cref="Idle::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Idle::Update">
    /// This method is called each frame to update this goal.  
    /// Slows down the AI to bring them to a stop (if they are moving).
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Idle::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

protected:
    Base::Vector3 m_TargetPosition;
    Collision::Handle  m_CollHandle;
    Bool          m_Finished;  // Goal has completed
};

