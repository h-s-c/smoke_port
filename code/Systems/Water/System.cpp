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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Water/System.hpp"
#include "Systems/Water/Scene.hpp"
#include "Systems/Water/Object.hpp"


///////////////////////////////////////////////////////////////////////////////
// WaterSystem - Constructor
WaterSystem::WaterSystem( void ) : ISystem()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~WaterSystem - Destructor
WaterSystem::~WaterSystem( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr WaterSystem::GetName( void )
{
    return System::Names::Water;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type WaterSystem::GetSystemType( void )
{
    return System::Types::Water;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error WaterSystem::Initialize( Properties::Array Properties )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void WaterSystem::GetProperties( Properties::Array& Properties )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void WaterSystem::SetProperties( Properties::Array Properties )
{
    ASSERT( m_bInitialized );
}


///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene* WaterSystem::CreateScene( void )
{
    return new WaterScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destorys the given Scene, free all associated resources
Error WaterSystem::DestroyScene( ISystemScene* pSystemScene )
{
    ASSERT( pSystemScene != NULL );

    WaterScene* pScene = reinterpret_cast<WaterScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

