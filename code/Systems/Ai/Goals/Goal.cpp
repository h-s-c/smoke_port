//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Ai/Bots/Bot.hpp"
#include "Systems/Ai/Goals/Goal.hpp"


///////////////////////////////////////////////////////////////////////////////
// Goal - Constructor with a given bot
Goal::Goal( Bot* pBot )
{
    m_Bot = pBot;
}


///////////////////////////////////////////////////////////////////////////////
// ~Goal - Destructor
Goal::~Goal( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Goal::PreUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Goal::Update( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}

///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Goal::PostUpdate( f32 DeltaTime )
{
    UNREFERENCED_PARAM( DeltaTime );
}

