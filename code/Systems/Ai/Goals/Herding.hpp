#pragma once

#include "Systems/Ai/Goals/Goal.hpp"


#define MAX_HERDING_TARGETS 256

struct HerdingTarget
{
    Bot*         m_pBot;
    Bool         m_Visible;
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Herding</c> Implementation of a herding goal.  This goal mimics
///   the herding behavior of pack animals (i.e. horse).
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Herding : public Goal
{
public:
    Herding( Bot* pBot );
    virtual ~Herding( void );

    /// <summary cref="Herding::GetName">
    ///   Gets the name for this goal ("Herding").
    /// </summary>
    /// <returns>pcstr - Name of this goal.</returns>
    /// <seealso cref="Goal::GetName"/>
    virtual pcstr GetName( void ) { return "Herding"; }

    /// <summary cref="Herding::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Herding::Update">
    /// This method is called each frame to update this goal.  
    /// Updates herding targets and determines the best velocity to herd 
    /// (Avoidance + Matching + Centering).
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Herding::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.
    /// Does nothing for this goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Goal::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

protected: 
    /// <summary cref="Herding::Avoidance">
    /// Determines the best vector to avoid all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be avoided.</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Avoidance( HerdingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );

    /// <summary cref="Herding::Matching">
    /// Determines the best vector to match the heading of all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be followed.</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Matching( HerdingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );

    /// <summary cref="Herding::Centering">
    /// Determines the best vector to get to the center of all p_Targets.
    /// </summary>
    /// <param name="p_Targets">An array of target that should be centered upon</param>
    /// <param name="NumTargets">The number of targets in p_Targets.</param>
    /// <param name="Result">This vector will be filled in with the result.</param>
    void Centering( HerdingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result );  // Determine centering vector

    /// <summary cref="Herding::FindTargets">
    /// Find suitable herding targets (other AI's we should herd with).
    /// </summary>
    void FindTargets( void );

    HerdingTarget m_Targets[ MAX_HERDING_TARGETS ];  // All possible targets
    u32           m_NumTargets;                      // Number of possible targets
    f32           m_Range;                           // Perception range
    f32           m_MinDistance;                     // Desired min distance from targets
};

