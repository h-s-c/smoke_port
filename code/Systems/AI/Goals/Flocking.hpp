#pragma once

#include "Systems/AI/Goals/Goal.hpp"


#define MAX_FLOCKING_TARGETS 256

struct FlockingTarget
{
    Bot* m_pBot;
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Flocking</c> Implementation of a flocking goal.  This goal mimics
///   the flocking behavior of birds and schooling behavior of fish.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Flocking : public Goal
{
public:
    Flocking( Bot* pBot );
    virtual ~Flocking( void );

    /// <summary cref="Flocking::GetName">
    ///   Gets the name for this goal ("Flocking").
    /// </summary>
    /// <returns>pcstr - Name of this goal.</returns>
    /// <seealso cref="Goal::GetName"/>
    virtual pcstr GetName( void ) { return "Flocking"; }

    /// <summary cref="Flocking::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Flocking::Update">
    /// This method is called each frame to update this goal.
    /// Updates flocking targets and determines the best velocity to flock with them 
    /// (Avoidance + Matching + Centering).
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Flocking::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

protected:
    /// <summary cref="Flocking::Avoidance">
    /// Determines the best vector to avoid all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be avoided.</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Avoidance( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );

    /// <summary cref="Flocking::Matching">
    /// Determines the best vector to match the heading of all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be followed.</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Matching( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );

    /// <summary cref="Flocking::Centering">
    /// Determines the best vector to get to the center of all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be centered upon</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Centering( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );

    /// <summary cref="Flocking::FindTargets">
    /// Find suitable flocking targets (other AI's we should flock with).
    /// </summary>
    void FindTargets( void );

    FlockingTarget m_Targets[ MAX_FLOCKING_TARGETS ];  // All possible targets
    u32            m_NumTargets;                       // Number of possible targets
    f32            m_SqrdRange;                        // Perception range (squared)
    f32            m_MinSqrdDistance;                  // Desired min distance (squared) from targets
};

