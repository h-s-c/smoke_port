//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Scene.hpp"
#include "Systems/AI/Bots/Animal.hpp"
#include "Systems/AI/Bots/Bot.hpp"
#include "Systems/AI/Goals/Flocking.hpp"


///////////////////////////////////////////////////////////////////////////////
// Flocking - Default constructor
Flocking::Flocking( Bot* pBot ) : Goal( pBot )
{
    // Set default values - rand not thread safe?
    m_SqrdRange = 1500.0f * pBot->m_Perception;
    m_MinSqrdDistance = pBot->m_Radius + ( pBot->m_Radius * 0.1f * pBot->m_Perception );

    // Use squared distance for better performance
    m_SqrdRange *= m_SqrdRange;
    m_MinSqrdDistance *= m_MinSqrdDistance;


    // Find flocking targets
    FindTargets();
}


///////////////////////////////////////////////////////////////////////////////
// ~Flocking - Destructor
Flocking::~Flocking( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Flocking::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Flocking::Update( f32 DeltaTime )
{
    ASSERT( m_Bot->m_Type == BotType::e_Animal || m_Bot->m_Type == BotType::e_Chicken || m_Bot->m_Type == BotType::e_Horse || m_Bot->m_Type == BotType::e_Swallow );

    // Find all targets in range
    u32 NumTargets = 0;
    FlockingTarget* p_Targets[ MAX_FLOCKING_TARGETS ];
    for( u32 Index = 0; Index < m_NumTargets; Index++ )
    {
        Base::Vector3 Diff = m_Targets[ Index ].m_pBot->m_Position - m_Bot->m_Position;

        // Determine the squared distance
        f32 Distance = Diff.x * Diff.x + Diff.y * Diff.y + Diff.z * Diff.z;

        // Add this as a target, if it's in range and in front of us (or it is really close)
        if( Distance < m_SqrdRange 
         && ( m_Bot->m_Facing.Dot( Diff ) > 0.0f || Distance < ( m_MinSqrdDistance * 4.0f ) ) )
        {
            ASSERT( NumTargets < MAX_FLOCKING_TARGETS );
            p_Targets[ NumTargets ] = &m_Targets[ Index ];
            NumTargets++;
        }
    }

    if( NumTargets > 0 )
    {
        // Target Avoidance
        Base::Vector3 AvoidanceVector;
        Avoidance( p_Targets, NumTargets, AvoidanceVector );

        // Velocity Matching
        Base::Vector3 MatchingVector;
        Matching( p_Targets, NumTargets, MatchingVector );

        // Flock Centering
        Base::Vector3 CenterVector;
        Centering( p_Targets, NumTargets, CenterVector );

        // Determine the new direction
        Base::Vector3 Direction = AvoidanceVector + MatchingVector + CenterVector;
        
        // Remove y difference if we can't fly
        Animal* p_Animal = (Animal*)m_Bot;
        if( !p_Animal->m_CanFly )
        {
            // Don't want to change elevation
            Direction.y = 0;  
        }

        Direction.Normalize();

        // Adjust desired veleocity
        m_Bot->m_DesiredVelocity = m_Bot->m_DesiredVelocity + Direction;

        // Adjust animals speed
        if( p_Animal->m_CanFly )
        {
            // If this animal is flying, decelerate when flying up and accelerate when flying down
            f32 DesiredAccel = MAX( -m_Bot->m_Accel, ( m_Bot->m_Accel * -m_Bot->m_DesiredVelocity.y ) );
            f32 Accel = MIN( m_Bot->m_Accel, DesiredAccel );

            // Adjust the flying bot's speed
            f32 DesiredSpeed = MAX( ( m_Bot->m_Speed + ( Accel * DeltaTime ) ), ( m_Bot->m_MaxSpeed * 0.5f ) );
            m_Bot->m_Speed = MIN( m_Bot->m_MaxSpeed, DesiredSpeed );
        }
        else
        {
            // Adjust bot speed (accelerate to go as fast as we can)
            m_Bot->m_Speed = MIN( m_Bot->m_MaxSpeed, m_Bot->m_Speed + (m_Bot->m_Accel * DeltaTime) );
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Flocking::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Avoidance - Determine target avoidance vector
void Flocking::Avoidance( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result )
{
    Result = Base::Vector3::Zero;

    // Determine if we are too close to any other targets
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Base::Vector3 Diff = p_Targets[ Index ]->m_pBot->m_Position - m_Bot->m_Position;

        // Determine the squared distance
        f32 Distance = Diff.x * Diff.x + Diff.y * Diff.y + Diff.z * Diff.z;

        if( Distance < m_MinSqrdDistance )
        {
            // Push away from this target to avoid colliding
            Result = ( Result - Diff ) * 2.0f;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Matching - Determine velocity matching vector
void Flocking::Matching( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result )
{
    // Determine the avaerage velocity of all the targets
    Base::Vector3 Velocity = Base::Vector3::Zero;
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Velocity += p_Targets[ Index ]->m_pBot->m_Velocity;
    }

    Velocity = Velocity * ( 1.0f / NumTargets );

    // Determine matching vector
    Result = (Velocity - m_Bot->m_Velocity) * 0.1f;
}


///////////////////////////////////////////////////////////////////////////////
// Centering - Determine centering vector
void Flocking::Centering( FlockingTarget** p_Targets, u32 NumTargets, Base::Vector3& Result )
{
    // Determine the center of all the targets
    Base::Vector3 Center = Base::Vector3::Zero;
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Center += p_Targets[ Index ]->m_pBot->m_Position;
    }

    Center = Center * ( 1.0f / NumTargets );

    // Determine centering vector
    Result = ( Center - m_Bot->m_Position ) * 0.1f;
}


///////////////////////////////////////////////////////////////////////////////
// FindTargets - Find suitable flocking targets
void Flocking::FindTargets( void )
{
    // Reset the flocking target list
    m_NumTargets = 0;

    // Get object list for this scene
    AIScene* p_Scene = (AIScene*)m_Bot->GetSystemScene();
    std::vector<AIObject*> p_Objects = p_Scene->GetObjects();

    // Loop through each Object and find targets
    std::vector<AIObject*>::iterator it;
    for( it = p_Objects.begin(); it != p_Objects.end(); it++ )
    {
        Bot* p_Bot = (Bot*)*it;
        if( m_Bot != p_Bot )
        {
            // See is this is a similar bot
            // TODO - sort by distance
            if( m_Bot->m_Type == p_Bot->m_Type )
            {
                m_Targets[ m_NumTargets ].m_pBot = p_Bot;
                m_NumTargets++;
            }

            // Check if we've maxed out the number of flocking targets
            if( m_NumTargets == MAX_FLOCKING_TARGETS )
            {
                break;
            }
        }
    }
}

