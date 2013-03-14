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

//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Ai/Scene.hpp"
#include "Systems/Ai/Bots/Swallow.hpp"
#include "Systems/Ai/Goals/Flocking.hpp"
#include "Systems/Ai/Goals/Herding.hpp"
#include "Systems/Ai/Goals/Idle.hpp"
#include "Systems/Ai/Goals/Panic.hpp"


// Local constants
#define STATE_FLOCK 0
#define STATE_PANIC 1

#define MIN_DISTANCE_FOR_GROUND 400.0f

///////////////////////////////////////////////////////////////////////////////
// Swallow - Constructor
Swallow::Swallow( ISystemScene* pSystemScene, pcstr pszName ) : Animal( pSystemScene, pszName )
{
    m_Type = BotType::e_Swallow;
    m_State.SetState( STATE_FLOCK );

    m_Behavior = Interface::e_Behavior_Idle;

    // Set defualt values
    m_Accel      = 300.0f;
    m_Perception = 0.1f;
    m_MaxSpeed   = 500.0f + 200.0f * ( (f32)rand() / (f32)RAND_MAX );
    m_SpeedRot   = 5.0f;
    m_Radius     = 25.0f;
    m_PanicLevel = 0.80f;
    m_Agility    = 4.0f;

    m_CanFly = True;
    m_PhysicsMove = False;

    // HACK: Hard code target positions
    static u32 counter = 0;
    if( counter > 90 )
    {
        m_TargetPosition = Base::Vector3::Zero;
        m_TargetPosition.x = -389.88f;
        m_TargetPosition.y = 1037.78f;
        m_TargetPosition.z = 2261.59f;
    }
    else
    {
        m_TargetPosition = Base::Vector3::Zero;
        m_TargetPosition.x = -5000.0;
        m_TargetPosition.y = 500.0f;
        m_TargetPosition.z = 1500.0f;
    }
    counter++;
}


///////////////////////////////////////////////////////////////////////////////
// ~Swallow - Destructor
Swallow::~Swallow( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Swallow::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    Animal::PreUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Swallow::Update( f32 DeltaTime )
{
    // HACK: Swallows only fly
    SetBehavior( Interface::e_Behavior_Flying );

    // Swallow State Machine
    switch( m_State.GetState() )
    {
    case STATE_FLOCK:
        {
            // Handle inital state change
            if( m_State.Triggered() )
            {
                // Delete old goal
                if( m_Goal )
                {
                    delete m_Goal;
                }

                // Create new goal
                m_Goal = (Goal*)new Flocking( this );
            }

            UpdateFlock();
        }
        break;

    case STATE_PANIC:
        {
            // Handle inital state change
            if( m_State.Triggered() )
            {
                // Delete old goal
                if( m_Goal )
                {
                    delete m_Goal;
                }

                // Create new goal
                m_Goal = (Goal*)new Panic( this );
            }

            UpdatePanic();
        }
        break;
    }

    // Reset fear each frame
    m_Fear = 0.0f;

    // Call base Update
    Animal::Update( DeltaTime );  

    // If we are too close to the ground and going down, set desired velocity to avoid the ground
    Base::Vector3 Down = Base::Vector3::Zero;
    Down.y = -1.0f;

    if(  m_Position.y < MIN_DISTANCE_FOR_GROUND && m_DesiredVelocity.Dot( Down ) > 0.0f )
    {
        m_DesiredVelocity = Base::Vector3::UnitY;
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Swallow::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    Animal::PostUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFlock - Update for the Flock state
void Swallow::UpdateFlock( void )
{
    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }

    // Add a slight desire to return to the target position
    Base::Vector3 Direction = m_TargetPosition - m_Position;
    Direction.Normalize();
    Direction *= 0.35f;
    m_DesiredVelocity = m_DesiredVelocity + Direction;
}


///////////////////////////////////////////////////////////////////////////////
// UpdatePanic - Update for the Panic state
void Swallow::UpdatePanic( void )
{
    // Check if we have calmed down
    if( m_Fear < m_PanicLevel || m_State.GetTime() > 2.0f )
    {
        // Calm down and seek nearby flock mates
        m_State.SetState( STATE_FLOCK );
    }
}

