//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Bots/Animal.hpp"
#include "Systems/AI/Bots/Bot.hpp"
#include "Systems/AI/Goals/Panic.hpp"


///////////////////////////////////////////////////////////////////////////////
// Panic - Default constructor
Panic::Panic( Bot* pBot ) : Goal( pBot )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~Panic - Destructor
Panic::~Panic( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Panic::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Panic::Update( f32 DeltaTime )
{
    ASSERT( m_Bot->m_Type == BotType::e_None || m_Bot->m_Type == BotType::e_Chicken || m_Bot->m_Type == BotType::e_Horse || m_Bot->m_Type == BotType::e_Swallow );
    Animal* p_Animal = (Animal*)m_Bot;

    // Adjust desired veleocity
    m_Bot->m_DesiredVelocity = m_Bot->m_DesiredVelocity + p_Animal->m_FearVector;
    m_Bot->m_DesiredVelocity.Normalize();

    // Adjust bot speed (go as fast as we can)
    m_Bot->m_Speed = MIN( m_Bot->m_MaxSpeed, m_Bot->m_Speed + (m_Bot->m_Accel * DeltaTime) );
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Panic::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


