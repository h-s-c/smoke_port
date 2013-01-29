//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Explosion/Explosion.hpp"
#include "Systems/Explosion/System.hpp"

// Local data
pcstr Explosion::sm_kapszPropertyNames[] =
{
    "Force", // Currently unused, Perhaps Kaboom will make use of these properties
    "Radius" // Currently unused, Perhaps Kaboom will make use of these properties
};

const Properties::Property Explosion::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Force ],
                          VALUE2( Properties::Values::Vector3, Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          NULL, NULL, NULL, NULL,
                          "", 0.0f, 0.0f, 0.0f ),
    Properties::Property( sm_kapszPropertyNames[ Property_Radius ],
                          VALUE2( Properties::Values::Vector3, Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::Multiple,
                          NULL, NULL, NULL, NULL,
                          "", 0.0f, 0.0f, 0.0f ),
};

///////////////////////////////////////////////////////////////////////////////
// Explosion - Constructor
Explosion::Explosion( ISystemScene* pSystemScene, pcstr pszName ) : ExplosionObject( pSystemScene, pszName )
{
    // Set default values
    m_Type = ExplosionType::e_None;
    m_BlastForce = 0.0f;
    m_BlastRadius = 0.0f;
    m_Velocity = Math::Vector3::Zero;
}


///////////////////////////////////////////////////////////////////////////////
// ~Explosion - Destructor
Explosion::~Explosion( void )
{
}
///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occured (change handler)
Error Explosion::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    // Call parent handler
    Error Result = ExplosionObject::ChangeOccurred( pSubject, ChangeType );

    return Result;
}


///////////////////////////////////////////////////////////////////////////////
// PreUpdate - PreUpdate processing
void Explosion::PreUpdate( f32 DeltaTime )
{
    // Call base PreUpdate
    ExplosionObject::PreUpdate( DeltaTime );
}


///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void Explosion::Update( f32 DeltaTime )
{
    // Call base Update
    ExplosionObject::Update( DeltaTime );
}

///////////////////////////////////////////////////////////////////////////////
void Explosion::SetVelocity(Math::Vector3 velocity)
{
    m_Velocity = velocity;
}
///////////////////////////////////////////////////////////////////////////////
const Math::Vector3* Explosion::GetVelocity(void)
{
    return &m_Velocity;
}

///////////////////////////////////////////////////////////////////////////////
const Math::Vector3* Explosion::GetPosition(void)
{
    return &m_Position;
}


///////////////////////////////////////////////////////////////////////////////
// PostUpdate - PostUpdate processing
void Explosion::PostUpdate( f32 DeltaTime )
{
    // Call base PostUpdate
    ExplosionObject::PostUpdate( DeltaTime );
}

///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Explosion
System::Changes::BitMask Explosion::GetPotentialSystemChanges( void )
{
    return ( System::Changes::Geometry::Position
           | System::Changes::Geometry::Orientation );
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Get properties for this Explosion
void Explosion::GetProperties( Properties::Array& Properties )
{
    // Add all the properties.
    Properties.reserve( Properties.size() + Explosion::Property_Count );

    for( i32 i=0; i < Explosion::Property_Count; i++ )
    {
        Properties.push_back( Explosion::sm_kaDefaultProperties[ i ] );
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Explosion
void Explosion::SetProperties( Properties::Array Properties )
{
    // Read in the properties
    for( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if( it->GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = it->GetName();
            if( sName == Explosion::sm_kapszPropertyNames[ Property_Force ] )
            {
                // Currently unused, Perhaps Kaboom will make use of these properties
            }
            else if( sName == Explosion::sm_kapszPropertyNames[ Property_Radius ] )
            {
                // Currently unused, Perhaps Kaboom will make use of these properties
            }
            else
            {
                ASSERT( False );  // Invalid property
            }

            // Set this property to invalid since it's already been read.
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}

