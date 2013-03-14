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
#include "Systems/Ai/System.hpp"
#include "Systems/Ai/Bots/Animal.hpp"


///////////////////////////////////////////////////////////////////////////////
// Animal - Constructor
Animal::Animal( ISystemScene* pSystemScene, pcstr pszName ) : Bot( pSystemScene, pszName )
{
    // Set default values
    m_Type = BotType::e_Animal;

    m_Fear = 0.0f;
    m_PanicLevel = 0.5f;
    m_FearVector = Base::Vector3::Zero; 

    m_CanFly = False;
}


///////////////////////////////////////////////////////////////////////////////
// ~Bot - Destructor
Animal::~Animal( void )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Animal::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    Bot::PreUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Animal::Update( f32 DeltaTime )
{
    // Call base Update
    Bot::Update( DeltaTime );

    // Update fear level
    UpdateFear( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Animal::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    Bot::PostUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFear - Update the fear level of this animal
void Animal::UpdateFear( f32 DeltaTime )
{
    // Clear fear vector if we are no longer afraid
    if( m_Fear == 0.0f )
    {
        m_FearVector = Base::Vector3::UnitX; 
    }

    // Search for all scary things
    AIScene* pScene = (AIScene*)GetSystemScene();
    std::list<POI*> POIs = pScene->GetPOI();

    for( std::list<POI*>::iterator it = POIs.begin(); it != POIs.end(); it++ )
    {
        POI* pPOI = *it;
        Base::Vector3 Diff = m_Position - pPOI->GetPosition();

        // Handle contacts
        if( pPOI->GetType() == POIType::e_POI_Fire )
        {
            POIFire* pPOIFire = (POIFire*)pPOI;

            // Determine if we are close to the fire
            Base::Vector3 Min, Max;
            pPOIFire->GetAABB( Min, Max );
            Min = pPOIFire->GetPosition() + ( Min - pPOIFire->GetPosition() ) * 4.0f;
            Max = pPOIFire->GetPosition() + ( Max - pPOIFire->GetPosition() ) * 4.0f;

            if( m_Position.x > Min.x && m_Position.x < Max.x
             && m_Position.y > Min.y && m_Position.y < Max.y
             && m_Position.z > Min.z && m_Position.z < Max.z )
            {
                // Increase fear based on how close we are to the center
                f32 MaxDistance = ( Max - pPOIFire->GetPosition() ).Magnitude();
                f32 Distance = Diff.Magnitude();

                f32 Factor = (MaxDistance - Distance) / MaxDistance;
                m_Fear = Base::Min( 1.0f, m_Fear + Factor );
                m_FearVector += Diff.Normalize() * Factor * 4.0f;
            }
        }
        else if( pPOI->GetType() == POIType::e_POI_Contact )
        {
            //POIContact* pPOIContact = (POIContact*)pPOI;

            f32 Distance = Base::Max( 0.0f, ( Diff.Magnitude() / ( 200.0f * m_Perception ) ) - 1.0f );
            if( Distance < 4.0f )
            {
                m_FearVector += Diff.Normalize() / Distance;
                m_Fear = Base::Min( 1.0f, m_Fear + ( 1.0f / Distance ) );
            }
        }
    }

    // Normalize and clamp y if we can't fly
    if( !m_CanFly )
    {
        m_FearVector.y = 0.0f;
    }
    m_FearVector.Normalize();

    // Cool down fear
    m_Fear = Base::Max( 0.0f, m_Fear - ( 0.1f * DeltaTime ) );
}

