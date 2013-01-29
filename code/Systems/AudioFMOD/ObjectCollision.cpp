//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/ObjectCollision.hpp"
#include "Systems/AudioFMOD/System.hpp"
//stdlib
#include <cstring>
//external
#include "fmod.hpp"

pcstr FMODObjectCollision::sm_kapszPropertyNames[] =
{
    "CollisionSF", "CollisionVolume", "CollisionPriority",
};

const Properties::Property FMODObjectCollision::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_CollisionSF ],
                          VALUE3( Properties::Values::String, Properties::Values::String, 
                          Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "", "", "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_CollisionVolume ],
                          VALUE1( Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1.0f ),
    Properties::Property( sm_kapszPropertyNames[ Property_CollisionPriority ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
};

///////////////////////////////////////////////////////////////////////////////
// FMODObjectCollision - Constructor
FMODObjectCollision::FMODObjectCollision(
    ISystemScene* pSystemScene
    )
    : FMODObject( pSystemScene )
    , f_CollisionVolume ( 1.0f )
    , i_CollisionPriority ( 0 )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );
}

///////////////////////////////////////////////////////////////////////////////
// ~FMODObjectCollision - Destructor
FMODObjectCollision::~FMODObjectCollision(
    void
    )
{
}

///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Object with the given properties
Error
FMODObjectCollision::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Call base class.
    //
    FMODObject::Initialize( Properties );
    
    if ( m_pSystem->AudioInitialized() ) 
    {
        FMOD_VECTOR sourcePos = { m_Position[0], m_Position[1], m_Position[2] };
        FMOD_VECTOR sourceVel = { 0.0, 0.0, 0.0 };
                  
        // Load sound in a paused state with proper attributes, when collision occurs
        // the sound will be unpaused
        FMODObject::LoadSound( CollisionSFName, &CollisionBuffer );
        m_Result = m_pSystem->m_pFMOD->playSound(FMOD_CHANNEL_FREE, CollisionBuffer, true, &CollisionSource);
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = CollisionSource->setChannelGroup( FMODSystem::stateStopped );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = CollisionSource->set3DAttributes( &sourcePos, &sourceVel );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = CollisionSource->setPriority( i_CollisionPriority );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        m_Result = CollisionSource->setVolume( f_CollisionVolume );
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
    } // end if (m_pSystem->AudioInitialized())

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Object are returned in Properties
void
FMODObjectCollision::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Call base class.
    //
    FMODObject::GetProperties( Properties );

    ////
    //// Add all the properties.
    ////
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }
}

///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Object
void
FMODObjectCollision::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );

    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        std::string sName = it->GetName();

        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            //
            // Object Properties
            // These are sound file dependant
            //
            if ( sName == sm_kapszPropertyNames[ Property_CollisionSF ] )
            {
                strcpy_s( CollisionSFName, sizeof( CollisionSFName ), it->GetStringPtr( 0 ) );
                
                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_CollisionVolume ] )
            {
                f_CollisionVolume = it->GetFloat32( 0 );

                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_CollisionPriority ] )
            {
                i_CollisionPriority = it->GetInt32( 0 );
                
                //
                // Set this property to invalid since it's already been read.
                //
                it->ClearFlag( Properties::Flags::Valid );
            }
        }
    }

    //
    // Let the base class process the properties.
    //
    FMODObject::SetProperties( Properties );
}

///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns the systems changes this Object 
//                           is intrested in hearing about
System::Changes::BitMask
FMODObjectCollision::GetDesiredSystemChanges(
    void
    )
{
    return ( FMODObject::GetDesiredSystemChanges() | System::Changes::POI::Contact );
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)
Error
FMODObjectCollision::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    if (m_pSystem->AudioInitialized()) 
    {
        // Process changes
        if ( ChangeType & System::Changes::POI::Contact )
        {
            IContactObject* pContactObject = dynamic_cast<IContactObject*>(pSubject);
            ASSERT( pContactObject );
            const IContactObject::Info* pContactInfo = pContactObject->GetContact();

            if (pContactInfo->m_Impact > IMPACT_SPEED_SOUND)
            {
                // Update position of impact and unpause collision sound
                Math::Vector3 currPos = pContactInfo->m_Position;
                UpdateCollisionPosition( &currPos );
                m_Result = CollisionSource->setPaused( false );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = CollisionSource->setChannelGroup( FMODSystem::statePlaying );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
        }
    }

    // Pass changes on to base class
    FMODObject::ChangeOccurred( pSubject, ChangeType );

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Object
System::Changes::BitMask
FMODObjectCollision::GetPotentialSystemChanges(
    void
    )
{
    return FMODObject::GetPotentialSystemChanges();
}

///////////////////////////////////////////////////////////////////////////////
// UpdateCollisionPosition - Update the position where the collision occurred
Error FMODObjectCollision::UpdateCollisionPosition( Math::Vector3* CurrPos )
{
    FMOD_VECTOR newPosition = {CurrPos->x, CurrPos->y, CurrPos->z};
    
    m_Result = CollisionSource->set3DAttributes( &newPosition, 0 );
    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );

    return Errors::Success;
}
