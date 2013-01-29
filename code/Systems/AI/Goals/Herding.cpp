//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Scene.hpp"
#include "Systems/AI/Bots/Animal.hpp"
#include "Systems/AI/Bots/Bot.hpp"
#include "Systems/AI/Goals/Herding.hpp"


///////////////////////////////////////////////////////////////////////////////
// Herding - Default constructor
Herding::Herding( Bot* pBot ) : Goal( pBot )
{
    // Set default values
    m_Range = 1000.0f + 1000.0f * pBot->m_Perception;
    m_MinDistance = pBot->m_Radius + ( pBot->m_Radius * 0.1f * pBot->m_Perception );

    // Find herding targets
    FindTargets();
}


///////////////////////////////////////////////////////////////////////////////
// ~Herding - Destructor
Herding::~Herding( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Herding::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Herding::Update( f32 DeltaTime )
{
    ASSERT( m_Bot->m_Type == BotType::e_Animal || m_Bot->m_Type == BotType::e_Chicken || m_Bot->m_Type == BotType::e_Horse || m_Bot->m_Type == BotType::e_Swallow );

    // Find all targets in range
    u32 NumTargets = 0;
    HerdingTarget* p_Targets[ MAX_HERDING_TARGETS ];
    for( u32 Index = 0; Index < m_NumTargets; Index++ )
    {
        // If this target is visiable
        if( m_Targets[ Index ].m_Visible )
        {
            Math::Vector3 Diff = m_Targets[ Index ].m_pBot->m_Position - m_Bot->m_Position;

            // Add this as a target if it's in range
            if( Diff.Magnitude() < m_Range )
            {
                p_Targets[ NumTargets ] = &m_Targets[ Index ];
                NumTargets++;
            }
        }
    }

    if( NumTargets > 0 )
    {
        // Target Avoidance
        Math::Vector3 AvoidanceVector;
        Avoidance( p_Targets, NumTargets, AvoidanceVector );

        // Velocity Matching
        Math::Vector3 MatchingVector;
        Matching( p_Targets, NumTargets, MatchingVector );

        // Flock Centering
        Math::Vector3 CenterVector;
        Centering( p_Targets, NumTargets, CenterVector );

        // Determine the new direction
        Math::Vector3 Direction = AvoidanceVector + MatchingVector + CenterVector;
        
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

        // Adjust bot speed (go as fast as we can)
        m_Bot->m_Speed = MIN( m_Bot->m_MaxSpeed, m_Bot->m_Speed + (m_Bot->m_Accel * DeltaTime) );
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Herding::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Avoidance - Determine target avoidance vector
void Herding::Avoidance( HerdingTarget** p_Targets, u32 NumTargets, Math::Vector3& Result )
{
    Result = Math::Vector3::Zero;

    // Determine if we are too close to any other targets
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Math::Vector3 Diff = p_Targets[ Index ]->m_pBot->m_Position - m_Bot->m_Position;

        if( Diff.Magnitude() < m_MinDistance && m_Bot->m_Facing.Dot( Diff ) > 0.0f )
        {
            // Push away from this target to avoid colliding
            Result = ( Result - Diff ) * 0.25f;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Matching - Determine velocity matching vector
void Herding::Matching( HerdingTarget** p_Targets, u32 NumTargets, Math::Vector3& Result )
{
    // Determine the avaerage velocity of all the targets
    Math::Vector3 Velocity = Math::Vector3::Zero;
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Velocity += p_Targets[ Index ]->m_pBot->m_Velocity;
    }

    Velocity = Velocity * ( 1.0f / NumTargets );

    // Determine matching vector
    Result = (Velocity - m_Bot->m_Velocity) * 0.5f;
}


///////////////////////////////////////////////////////////////////////////////
// Centering - Determine centering vector
void Herding::Centering( HerdingTarget** p_Targets, u32 NumTargets, Math::Vector3& Result )
{
    // Determine the center of all the targets
    Math::Vector3 Center = Math::Vector3::Zero;
    for( u32 Index = 0; Index < NumTargets; Index++ )
    {
        Center += p_Targets[ Index ]->m_pBot->m_Position;
    }

    Center = Center * ( 1.0f / NumTargets );

    // Determine centering vector
    Result = ( Center - m_Bot->m_Position ) * 0.05f;
}


///////////////////////////////////////////////////////////////////////////////
// FindTargets - Find suitable herding targets
void Herding::FindTargets( void )
{
    // Reset the herding target list
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
                m_Targets[ m_NumTargets ].m_Visible = True;
                m_NumTargets++;
            }

            // Check if we've maxed out the number of herding targets
            if( m_NumTargets == MAX_HERDING_TARGETS )
            {
                break;
            }
        }
    }
}

