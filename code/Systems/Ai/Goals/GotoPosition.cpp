//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Ai/Bots/Bot.hpp"
#include "Systems/Ai/Goals/GotoPosition.hpp"


///////////////////////////////////////////////////////////////////////////////
// GotoPosition - Constructor with target position
GotoPosition::GotoPosition( Bot* pBot ) : Goal( pBot )
{
    // Init instance data
    m_TargetPosition = Base::Vector3::Zero;
    m_Finished = False;
}


///////////////////////////////////////////////////////////////////////////////
// GotoPosition - Constructor with target position
GotoPosition::GotoPosition( Bot* pBot, Base::Vector3 TargetPosition ) : Goal( pBot )
{
    // Init instance data
    SetPosition( TargetPosition );
    m_Finished = False;
}


///////////////////////////////////////////////////////////////////////////////
// ~GotoPosition - Destructor
GotoPosition::~GotoPosition( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// SetPosition - Set target position
void GotoPosition::SetPosition( Base::Vector3 TargetPosition )
{
    m_TargetPosition = TargetPosition;
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void GotoPosition::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void GotoPosition::Update( f32 DeltaTime )
{
    if( !m_Finished )
    {
        Base::Vector3 Disp = m_TargetPosition - m_Bot->m_Position;
        f32 DistToTravel = Disp.Magnitude();

        f32 TimeToStop = m_Bot->m_Speed / m_Bot->m_Accel;
        f32 DistToStop = m_Bot->m_Speed * TimeToStop;

        // Adjust bot speed
        if( DistToStop < DistToTravel )
        {
            // Accelerate
            m_Bot->m_Speed = MIN( m_Bot->m_MaxSpeed, m_Bot->m_Speed + (m_Bot->m_Accel * DeltaTime) );
        }
        else
        {
            // Decelerate
            m_Bot->m_Speed = MAX( 0.0f, m_Bot->m_Speed - (m_Bot->m_Accel * DeltaTime) );
        }

        // Normalize the displacement
        Disp.x = Disp.x / DistToTravel;
        Disp.y = Disp.y / DistToTravel;
        Disp.z = Disp.z / DistToTravel;

        // Adjust desired veleocity
        m_Bot->m_DesiredVelocity = m_Bot->m_DesiredVelocity + Disp;
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void GotoPosition::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );

    // Check if we've reached the target position
    Base::Vector3 Disp = m_TargetPosition - m_Bot->m_Position;

    #define MIN_DISTANCE_FROM_TARGET 1.0f
    if( Disp.Magnitude() < MIN_DISTANCE_FROM_TARGET )
    {
        m_Finished = True;
        m_Bot->m_Speed = 0.0f;
        m_Bot->m_Velocity = Base::Vector3::Zero;
    }
}

