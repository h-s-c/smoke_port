//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AI/Bots/Animal.hpp"
#include "Systems/AI/Bots/Bot.hpp"
#include "Systems/AI/Goals/Idle.hpp"


///////////////////////////////////////////////////////////////////////////////
// Idle - Default constructor
Idle::Idle( Bot* pBot ) : Goal( pBot )
{
}


///////////////////////////////////////////////////////////////////////////////
// ~Idle - Destructor
Idle::~Idle( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Idle::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Idle::Update( f32 DeltaTime )
{
    // Come to a stop
    m_Bot->m_Speed = Math::Max( 0.0f, m_Bot->m_Speed - (m_Bot->m_Accel * DeltaTime) );
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Idle::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}

