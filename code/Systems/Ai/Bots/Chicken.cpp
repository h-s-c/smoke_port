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
#include "Systems/Ai/Bots/Chicken.hpp"
#include "Systems/Ai/Goals/Flocking.hpp"
#include "Systems/Ai/Goals/Idle.hpp"
#include "Systems/Ai/Goals/Panic.hpp"


// Local constants
#define STATE_IDLE  0
#define STATE_FLOCK 1
#define STATE_PANIC 2

#define MAX_SPEED_CALM  35.0f
#define MAX_SPEED      100.0f

///////////////////////////////////////////////////////////////////////////////
// Chicken - Constructor
Chicken::Chicken( ISystemScene* pSystemScene, pcstr pszName ) : Animal( pSystemScene, pszName )
{
    m_Type = BotType::e_Chicken;
    m_State.SetState( STATE_IDLE );

    m_Behavior = Interface::e_Behavior_Idle;

    // Set defualt values
    m_Accel      = 50.0f;
    m_Perception = 0.5f;
    m_MaxSpeed   = MAX_SPEED;
    m_SpeedRot   = 5.0f;
    m_Radius     = 50.0f;
    m_YOffset    = 100.0f;
    m_PanicLevel = 0.40f;
    m_CurrentMaxSpeed = MAX_SPEED_CALM;

    m_IdleDirection = Base::Vector3::Zero;
    m_Duration = 0.0f;
}


///////////////////////////////////////////////////////////////////////////////
// ~Chicken - Destructor
Chicken::~Chicken( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Chicken::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    Animal::PreUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Chicken::Update( f32 DeltaTime )
{
    // Update max speed
    m_MaxSpeed = m_CurrentMaxSpeed;

    // Call base Update
    Animal::Update( DeltaTime );

    // Chicken State Machine
    switch( m_State.GetState() )
    {
    case STATE_IDLE:
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
                m_Goal = (Goal*)new Idle( this );

                // Set duration [0.5 to 1.5] second) - rand not thread safe?
                m_Duration = 0.5f + 1.0f * ( (f32)rand() / (f32)RAND_MAX );

                // Lower our current max speend
                m_CurrentMaxSpeed = MAX_SPEED_CALM;

                // Pick a random facing direction
                if( rand() % 16 == 0 )
                {
                    m_IdleDirection.x = -1.0f + 2.0f * ( (f32)rand() / (f32)RAND_MAX );
                    m_IdleDirection.z = -1.0f + 2.0f * ( (f32)rand() / (f32)RAND_MAX );
                    m_IdleDirection.Normalize();
                }
                else
                {
                    m_IdleDirection = m_Facing;
                }
            }

            UpdateIdle( DeltaTime );
        }
        break;

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

                // Increase our current max speend
                m_CurrentMaxSpeed = MAX_SPEED;
            }

            UpdatePanic();
        }
        break;
    }

    // HACK: Chickens only run or idle
    if( m_Speed > 0.0f )
    {
        SetBehavior( Interface::e_Behavior_Running );
    }
    else
    {
        SetBehavior( Interface::e_Behavior_Idle );
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Chicken::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    Animal::PostUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// UpdateIdle - Update for the Idle state
void Chicken::UpdateIdle( f32 DeltaTime )
{
    // Check if duration is up (change to flocking)
    if( m_State.GetTime() > m_Duration && m_Velocity.Magnitude() < 0.2f )
    {
        m_State.SetState( STATE_FLOCK );
        
        // Set duration [0.0 to 1.0] second)
        m_Duration = 1.0f * ( (f32)rand() / (f32)RAND_MAX );

        // Increase length of flocking as we get more panicked
        m_Duration += 2.0f * Base::Min( 1.0f, m_Fear / m_PanicLevel );
    }

    // Rotate toward our desired direction
    f32 Diff = Base::Angle::ACos( m_Facing.Dot( m_IdleDirection ) );
    if( Diff > 0.01f )
    {
        Base::Quaternion Rotation;
        Rotation.Set( (m_Facing.Cross( m_IdleDirection )).Normalize(), 0.25f * DeltaTime );

        m_Orientation = m_Orientation * Rotation;
        m_Facing = m_OriginalFacing;
        m_Orientation.Rotate( m_Facing );
        m_Facing.Normalize();
    }

    // Check if there is a panicked chicken in range
    AIScene* p_Scene = (AIScene*)GetSystemScene();
    std::vector<AIObject*> p_Objects = p_Scene->GetObjects();

    std::vector<AIObject*>::iterator it;
    for( it = p_Objects.begin(); it != p_Objects.end(); it++ )
    {
        Bot* p_Bot = (Bot*)*it;

        if( p_Bot != this 
         && p_Bot->m_Type == m_Type
         && p_Bot->GetState() == STATE_PANIC )
        {
            f32 Distance = (p_Bot->m_Position - m_Position).Magnitude();
            if( Distance < 1000.0f * m_Perception )
            {
                // A chicken near us is panicked, start flocking to follow it
                m_State.SetState( STATE_FLOCK );

                // Increase our current max speend
                m_CurrentMaxSpeed = MAX_SPEED;

                // Set duration [1.0 to 5.0] second)
                m_Duration = 1.0f + ( 4.0f * ( (f32)rand() / (f32)RAND_MAX ) );
            }
        }
    }

    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFlock - Update for the Flock state
void Chicken::UpdateFlock( void )
{
    // Check if duration is up (change to flocking)
    if( m_State.GetTime() > m_Duration )
    {
        m_State.SetState( STATE_IDLE );
    }

    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdatePanic - Update for the Panic state
void Chicken::UpdatePanic( void )
{
    // Check if we have calmed down
    if( m_Fear < m_PanicLevel )
    {
        // Calm down and seek nearby flock mates
        m_State.SetState( STATE_FLOCK );

        // Set duration [2.0 to 5.0] second)
        m_Duration = 2.0f + ( 3.0f * ( (f32)rand() / (f32)RAND_MAX ) );
    }
}

