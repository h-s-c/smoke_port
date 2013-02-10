//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Explosion/MeteorImpact.hpp"
#include "Systems/Explosion/System.hpp"
#include "Systems/Explosion/Scene.hpp"

// Defines
#define MAX_FRAGMENTS 3

// Statics
static int ballCounter = 0; // Cheat used alongside delta time to control release of meteor fragments

// Globals
POI* pLastContact = NULL; // Used to differentiate between meteor impact collision events

///////////////////////////////////////////////////////////////////////////////
// MeteorImpact - Constructor
MeteorImpact::MeteorImpact( ISystemScene* pSystemScene, pcstr pszName ) : Explosion( pSystemScene, pszName )
{
    m_Type = ExplosionType::e_MeteorImpact;

    // Set default values
    m_bFragmentUsed = false;
    m_Impact = Math::Vector3::Zero;
}


///////////////////////////////////////////////////////////////////////////////
// ~MeteorImpact - Destructor
MeteorImpact::~MeteorImpact( void )
{
}


MeteorImpact::MeteorImpact( const MeteorImpact& rhs ) : Explosion( rhs.m_pSystemScene, "" )
{
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void MeteorImpact::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    Explosion::PreUpdate( DeltaTime );
}

///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void MeteorImpact::Update( f32 DeltaTime )
{
    // Call base Update
    Explosion::Update( DeltaTime );

    ExplosionScene* pScene = (ExplosionScene*)GetSystemScene();
    std::list<POI*> POIs = pScene->GetPOI();

    // Iterate through the list of contact points of interest and release up to 3 
    // meteor fragments for this impact event and reset for the next event.
    // Note: meteor fragments are "pooled" meaning that there are M many meteors
    // and N fragments with N as the amount of fragments contained in the
    // scene definition file (.cdf).
    for( std::list<POI*>::iterator it = POIs.begin(); it != POIs.end(); it++ )
    {
        POI* pPOI = *it;
        Math::Vector3 Diff = m_Position - pPOI->GetPosition();
        if( pPOI->GetType() == POIType::e_POI_Contact )
        {   
            POIContact* pPOIContact = (POIContact*)pPOI;
            Math::Vector3 position = pPOIContact->GetPosition();
            f32 impact = pPOIContact->GetImpact();

            // Hack: As there is no differentiation between hitting the ground or any other static mesh
            // from Havok, we release fragments based on impact speed, if the object is a static mesh
            // at the time of impact (the house becomes dynamic after a collision), and if an unused
            // fragment from the pool is free up to the maximum allowed fragments per meteor impact.
            if( (impact > 800.0f ) && pPOIContact->IsStatic() && !m_bFragmentUsed )
            {
                if( pLastContact == pPOI )
                {
                    ballCounter = 0;
                    return;
                }

                if( ballCounter > (MAX_FRAGMENTS-1) )
                {
                    // Hack: remember this meteor impact collision event so we do not reinterpret future 
                    // impacts of qualifying velocity as a new impact (some meteors start so high
                    // that a secondary bounce still possesses a significant velocity)

                    pLastContact = pPOI; 
                    return;
                }
                else
                {
                    ballCounter++;
                }

                // Hack: The current architecture does not support object creation, rather objects exist in the scene
                // and we simply move them from below the world, position them at the point of impact with an
                // approximated deflection vector, and give each fragment an arc offset from the initial impact
                // around the meteor, give it a new deflection vector, and update related systems.
                m_bFragmentUsed = True;
                m_Position.x = sin(.707f*ballCounter); // assign this fragment a position away from other objects around the meteor
                m_Position.z = cos(.707f*ballCounter);
                m_Position.Normalize();
                m_Position *= 150;
                
                position.y += 100.0f*ballCounter; // reposition the meteor above the ground

                m_Position = position;

                Math::Vector3 inboundVelocity, deflectionVelocity;
                
                Math::Vector3 vectorI = pPOIContact->GetVelocityObjectA() + pPOIContact->GetVelocityObjectB();
                vectorI.y *= -1.0f;
                vectorI.x *= 0.95f;
                vectorI.Normalize(); // get the deflection adjusted vector

                const f32 scale = 0.1f;
                f32 x = Math::Random::GetRandomFloat( -scale, scale ); 
                f32 y = Math::Random::GetRandomFloat( -scale, scale ); 
                f32 z = Math::Random::GetRandomFloat( -scale, scale ); 
                f32 m = Math::Random::GetRandomFloat( -scale, scale ); 
                vectorI.x += x;
                vectorI.y += y;
                vectorI.z += z;
                impact += impact*m; // assign the fragment an adjusted deflection vector
                vectorI.Normalize();
               
                SetVelocity( vectorI * impact/2 ); // adjust the fragment velocity and scale it down for realism

                // Post these changes to update the physics and geometry systems because we've given
                // the meteor fragment a new position and velocity
                PostChanges( System::Changes::Geometry::Position );
                PostChanges( System::Changes::Physics::Velocity );
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void MeteorImpact::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    Explosion::PostUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Explosion
System::Changes::BitMask MeteorImpact::GetPotentialSystemChanges( void )
{
    return Explosion::GetPotentialSystemChanges() | System::Changes::Physics::Velocity;
}
