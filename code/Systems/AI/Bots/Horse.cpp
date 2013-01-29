//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Scene.hpp"
#include "Systems/AI/Bots/Horse.hpp"
#include "Systems/AI/Goals/Flocking.hpp"
#include "Systems/AI/Goals/Herding.hpp"
#include "Systems/AI/Goals/Idle.hpp"
#include "Systems/AI/Goals/Panic.hpp"


// Local constants
#define STATE_IDLE   0
#define STATE_WANDER 1
#define STATE_FLOCK  2
#define STATE_HERD   3
#define STATE_PANIC  4

///////////////////////////////////////////////////////////////////////////////
// Horse - Constructor
Horse::Horse( ISystemScene* pSystemScene, pcstr pszName ) : Animal( pSystemScene, pszName )
{
    m_Type = BotType::e_Horse;
    
    // ISMC HACK: Horse only herd and panic
    //m_State.SetState( STATE_IDLE );
    m_State.SetState( STATE_HERD );

    SetBehavior( Interface::e_Behavior_Idle );

    // Set defualt values
    m_Accel      = 70.0f + 20.0f * ( (f32)rand() / (f32)RAND_MAX );
    m_Perception = 1.0f;
    m_Radius     = 200.0f;
    m_YOffset    = 100.0f;
    m_PanicLevel = 0.60f;
    m_Duration   = 0.0f;

    m_WalkSpeed = 100.0f + 400.0f * ( (f32)rand() / (f32)RAND_MAX );
    m_RunSpeed  = 1000.0f + 400.0f * ( (f32)rand() / (f32)RAND_MAX );
    m_MaxSpeed  = m_WalkSpeed;
    m_SpeedRot  = 1.0f;

    m_TargetPosition = Math::Vector3::Zero;
    m_TargetPosition.x = -1200.0f;
    m_TargetPosition.y = 500.0f;
    m_TargetPosition.z = 10.0f;
}


///////////////////////////////////////////////////////////////////////////////
// ~Horse - Destructor
Horse::~Horse( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Horse::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    Animal::PreUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Horse::Update( f32 DeltaTime )
{
    // Call base Update
    Animal::Update( DeltaTime );

    // State specific update
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

                // Set our speed to walk
                m_MaxSpeed = m_WalkSpeed;

                // Set duration [5.0 to 60.0] second) - rand not thread safe?
                m_Duration = 5.0f + 55.0f * ( (f32)rand() / (f32)RAND_MAX );
            }

            UpdateIdle();
        }
        break;

    case STATE_WANDER:
        {
            // Handle inital state change
            if( m_State.Triggered() )
            {
                // Delete old goal
                if( m_Goal )
                {
                    delete m_Goal;
                }

                // Create new goal (use panic but just go the way we are facing)
                m_Goal = (Goal*)new Panic( this );

                // Set our speed to between gallop and run
                //f32 HalfSpeed = ( m_WalkSpeed + m_RunSpeed ) / 2.0f;
                //m_MaxSpeed = HalfSpeed + HalfSpeed * ( (f32)rand() / (f32)RAND_MAX );
                m_MaxSpeed = m_RunSpeed;
                    
                // Set duration [10.0 to 30.0] second) - rand not thread safe?
                m_Duration = 10.0f + 20.0f * ( (f32)rand() / (f32)RAND_MAX );
            }

            UpdateWander();
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

                // Set our speed to walk
                m_MaxSpeed = m_RunSpeed;

                // Set duration [5.0 to 10.0] second) - rand not thread safe?
                m_Duration = 5.0f + 5.0f * ( (f32)rand() / (f32)RAND_MAX );
            }

            UpdateFlocking();
        }
        break;

    case STATE_HERD:
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
                m_Goal = (Goal*)new Herding( this );

                // Increase our speed to run 
                m_MaxSpeed = m_RunSpeed;
            }

            UpdateHerding();
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

                // Increase our speed to run 
                m_MaxSpeed = m_RunSpeed;
            }

            UpdatePanic();
        }
        break;
    }

    // Update behavior
    /*
    if( m_Speed > 0.0f )
    {
        SetBehavior( Interface::e_Behavior_Walking );
    }
    else if( m_Speed > m_WalkSpeed )
    {
        SetBehavior( Interface::e_Behavior_Running );
    }
    */

    // HACK: Horse only run
    m_Speed = MIN( m_MaxSpeed, m_Speed + (m_Accel * DeltaTime) );
    SetBehavior( Interface::e_Behavior_Running );
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Horse::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    Animal::PostUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFear - Update the fear level of this horse
void Horse::UpdateFear( f32 DeltaTime )
{
    // Call the base class method
    Animal::UpdateFear( DeltaTime );

    // If we are not near another horse, than increase fear level
    Bool NearHorse = False;

    // Check if there is a horse in range
    AIScene* p_Scene = (AIScene*)GetSystemScene();
    std::vector<AIObject*> p_Objects = p_Scene->GetObjects();

    std::vector<AIObject*>::iterator it;
    for( it = p_Objects.begin(); it != p_Objects.end(); it++ )
    {
        Bot* p_Bot = (Bot*)*it;

        if( p_Bot != this && p_Bot->m_Type == m_Type )
        {
            f32 Distance = (p_Bot->m_Position - m_Position).Magnitude();
            if( Distance < 1000.0f * m_Perception )
            {
                // A horse near us
                NearHorse = True;
                break;
            }
        }
    }

    // Increase fear if we are not near another horse
    if( !NearHorse )
    {
        // HACK: Disable
        //m_Fear = Math::Min( 1.0f, m_Fear + 0.25f );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateIdle - Update for the Idle state
void Horse::UpdateIdle( void )
{
    // Check if duration is up (change to flocking)
    if( m_State.GetTime() > m_Duration && m_Velocity.Magnitude() == 0.0f )
    {
        if( rand() % 16 == 0 )
        {
            m_State.SetState( STATE_WANDER );
        }
        else
        {
            m_State.SetState( STATE_FLOCK );
        }
    }

    // Check if we are near a panicked horse
    if( NearPanickedHorse() )
    {
        // Herding to follow panicked horse
        m_State.SetState( STATE_HERD );
    }


    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }

    
    // Set our behavior to idle (when we aren't moving)
    if( m_Speed == 0.0f )
    {
        SetBehavior( Interface::e_Behavior_Idle );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateWander - Update for the Wander state
void Horse::UpdateWander( void )
{
    // Check if duration is up (change to idle)
    if( m_State.GetTime() > m_Duration )
    {
        m_State.SetState( STATE_IDLE );
    }

    // Check if we are near a panicked horse
    if( NearPanickedHorse() )
    {
        // Herding to follow panicked horse
        m_State.SetState( STATE_HERD );
    }

    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFlocking - Update for the Flock state
void Horse::UpdateFlocking( void )
{
    // Check if duration is up (change to idle)
    if( m_State.GetTime() > m_Duration )
    {
        m_State.SetState( STATE_IDLE );
    }

    // Check if we are near a panicked horse
    if( NearPanickedHorse() )
    {
        // Herding to follow panicked horse
        m_State.SetState( STATE_HERD );
    }

    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateHerding - Update for the Herd state
void Horse::UpdateHerding( void )
{
    // Check if we are near a panicked horse
    if( !NearPanickedHorse() )
    {
        // No longer near a panicked horse, go idle
        // HACK: Horse only herd and panic
        //m_State.SetState( STATE_IDLE );
    }

    // Check if we should panic
    if( m_Fear > m_PanicLevel )
    {
        m_State.SetState( STATE_PANIC );
    }

    // HACK: Add a slight desire to return to the middle
    Math::Vector3 Direction = m_TargetPosition - m_Position;
    Direction.y = 0.0f;
    Direction.Normalize();
    Direction *= 0.25f;
    m_DesiredVelocity = m_DesiredVelocity + Direction;
}


///////////////////////////////////////////////////////////////////////////////
// UpdatePanic - Update for the Panic state
void Horse::UpdatePanic( void )
{
    // Check if we have calmed down
    if( m_Fear < m_PanicLevel )
    {
        // Calm down and go idle
        // HACK: Horse only herd and panic
        //m_State.SetState( STATE_IDLE );
        m_State.SetState( STATE_HERD );
    }
}


///////////////////////////////////////////////////////////////////////////////
// NearPanickedHorse - Returns true if we are near a panicked horse
Bool Horse::NearPanickedHorse( void )
{
    Bool NearPanickedHorse = False;

    // Check if there is a panicked horse in range
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
                // A horse near us is panicked
                NearPanickedHorse = True;
                break;
            }
        }
    }

    return NearPanickedHorse;
}

