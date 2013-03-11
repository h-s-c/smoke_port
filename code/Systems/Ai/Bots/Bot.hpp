#pragma once

// Interfaces
#include "Interfaces/Services/Collision.hpp"
// System
#include "Systems/Ai/Object.hpp"
#include "Systems/Ai/Goals/Goal.hpp"


// Foward declarations
namespace BotType {
    enum BotType
    {
        e_None,
        e_Animal,
        e_Chicken,
        e_Horse,
        e_Swallow,
    };
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Bot</c> Implementation of the base bot AI.  A bot is an AI that
///   can have Goals.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class Bot : public AIObject, public IMoveObject
{
public:
    Bot( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~Bot( void );

    /// <summary cref="Bot::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="AIObject::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="Bot::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.  
    /// Initializes parameters for this frame and calls PreUpdate for this Bot's Goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="AIObject::PreUpdate"/>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="Bot::Update">
    /// This method is called each frame to update this Bot.  
    /// This is where most of the work is done to update the Bot.  This will update
    /// the Bot's Goal and do things like determine the new velocity.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="AIObject::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="Bot::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.  
    /// Finilizes Bot updates for this frame (post changes to other systems).  This
    /// will call PostUpdate for the Bot's Goal.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="AIObject::PostUpdate"/>
    virtual void PostUpdate( f32 DeltaTime );

    /// <summary cref="Bot::GetVelocity">
    /// Implementation of the <c>IMoveObject::GetVelocity</c> function.
    /// This function returns the Bot's current velocity
    /// </summary>
    /// <returns>Base::Vector3* - Velocity of this Bot</returns>
    /// <seealso cref="IMoveObject::GetVelocity"/>
    virtual const Base::Vector3* GetVelocity() { return &m_Velocity; }

    /// <summary cref="Bot::GetMaxVelocity">
    /// Implementation of the <c>IMoveObject::GetMaxVelocity</c> function.
    /// This function returns the Bot's make speed (max velocity magnitude)
    /// </summary>
    /// <returns>f32 - Max speed of this Bot</returns>
    /// <seealso cref="IMoveObject::GetMaxVelocity"/>
    virtual f32 GetMaxVelocity() { return m_MaxSpeed; }

protected:
    /// <summary cref="Bot::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    ///   Identies the system changes that this subject could possibly make.
    /// </summary>
    /// <returns>System::Changes::BitMask - A bitmask of the possible system changes.</returns>
    /// <seealso cref="AIObject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    Goal* m_Goal;         // Current goal for this bot
    Bool  m_PhysicsMove;  // Should this bot move by the physics system

public:
    BotType::BotType m_Type;  // Type of bot

    f32   m_Accel;       // Acceleration
    f32   m_MaxSpeed;    // Max speed
    f32   m_Perception;  // How perceptive is this bot? (used to scale range of environment tests (0.0-1.0])
    f32   m_Radius;      // Radius of sphere that defines general size of this bot
    f32   m_Speed;       // Current speed
    f32   m_SpeedRot;    // Rotation speed
    f32   m_Agility;     // Scalar used for changing m_Velocity to m_DesiredVelocity
    f32   m_YOffset;     // Distance off the ground from m_Position to the root

    Base::Vector3 m_Velocity;         // Current velocity 
    Base::Vector3 m_DesiredVelocity;  // Desired velocity 

    Base::Vector3 m_OriginalFacing;   // The orignal facing vector (based on art file)
    Base::Vector3 m_Facing;           // Direction this bot is facing

private:
    Base::Vector3 m_Ground;          // Current ground position
    Collision::Handle  m_GroundTest;      // Current ground test
    Bool          m_GroundValid;     // Is m_Ground valid?
    Collision::Handle  m_MoveTest;        // Current move collision test
    Base::Vector3 m_TargetPosition;  // Desired position
};

