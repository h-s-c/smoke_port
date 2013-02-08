//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/Object.hpp"
#include "Systems/AudioFMOD/System.hpp"
//stdlib
#include <ctime>
#include <cstring>
#include <mutex>
//external
#include "fmod.hpp"
//FIXME
#include "Systems/Input/Object.hpp"

//
// capture current time for random play
//
clock_t FMODObject::prevTime = clock();

Math::Vector3 FMODObject::prevPos;


pcstr FMODObject::sm_kapszPropertyNames[] =
{
    "SoundFile", "Listener", "Source", "Looping", "Random", "Intermittent", "Fire",
    "Water", "Position", "Orientation", "Volume", "Priority",
};

const Properties::Property FMODObject::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_SoundFile ],
                          VALUE4( Properties::Values::String, Properties::Values::String, 
                          Properties::Values::String, Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "", "", "", "" ),
    Properties::Property( sm_kapszPropertyNames[ Property_Listener ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Source ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Looping ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Random ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Intermittent ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ), 
    Properties::Property( sm_kapszPropertyNames[ Property_Fire ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Water ],
                          VALUE1( Properties::Values::Boolean ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          False ),
    Properties::Property( sm_kapszPropertyNames[ Property_Volume ],
                          VALUE1( Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          1.0f ),
    Properties::Property( sm_kapszPropertyNames[ Property_Priority ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_Position ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          Math::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Orientation ],
                          Properties::Values::Quaternion,
                          Properties::Flags::Valid,
                          NULL, NULL, NULL, NULL,
                          Math::Vector3::Zero ),
};

///////////////////////////////////////////////////////////////////////////////
// FMODObject - Constructor
FMODObject::FMODObject(
    ISystemScene* pSystemScene
    )
    : ISystemObject( pSystemScene, NULL )
    , m_bListener ( 0 )
    , m_bSource ( 0 )
    , m_bLooping ( 0 )
    , m_bRandom ( 0 )
    , m_bIntermittent ( 0 )
    , m_bFire ( 0 )
    , m_bWater ( 0 )
    , m_bCollisionOff ( 0 )
    , m_fVolume ( 1.0f )
    , m_iPriority ( 0 )
    , m_Position( Math::Vector3::Zero )
    , m_Orientation( Math::Quaternion::Zero )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Source = NULL;
    m_szSoundFileName[0] = 0;
}

///////////////////////////////////////////////////////////////////////////////
// ~FMODObject - Destructor
FMODObject::~FMODObject(
    void
    )
{
    // FMOD cleans up for itself
}

///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Object
System::Type
FMODObject::GetSystemType(
    void
    )
{
    return System::Types::Audio;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Object with the given properties
Error
FMODObject::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    //
    // Set this set as initialized.
    //
    m_bInitialized = True;

    //
    // Set the properties for this object.
    //
    SetProperties( Properties );

    //
    // Get system
    //
    m_pSystem = (FMODSystem*)this->GetSystemScene()->GetSystem();

    if ( m_pSystem->AudioInitialized() && m_pSystem->m_pFMOD != NULL ) 
    {
        if (m_bListener) 
        {
            // Set listener position and orientation
            FMOD_VECTOR ListenerPos = { m_Position[0], m_Position[1], m_Position[2] };
            FMOD_VECTOR ListenerVel = { 0.0, 0.0, 0.0 };
            FMOD_VECTOR ListenerFor = { 0.0, 0.0, -1.0 };
            FMOD_VECTOR ListenerUp = { 0.0, 1.0, 0.0 };

            m_Result = m_pSystem->m_pFMOD->set3DListenerAttributes( 0, &ListenerPos, &ListenerVel, &ListenerFor, &ListenerUp );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        }
        if (m_bSource && strcmp(m_szSoundFileName, "" ) != 0 ) 
        {
            // Set source position and velocity
            FMOD_VECTOR SourcePos = { m_Position[0], m_Position[1], m_Position[2] };
            prevPos = m_Position;
            FMOD_VECTOR SourceVel = { 0.0, 0.0, 0.0 };

            Error Err = LoadSound( m_szSoundFileName, &m_Buffer );
            UNREFERENCED_PARAM( Err );

            m_Result = m_Buffer->setLoopCount( FMOD_LOOP_FOREVER * m_bLooping);
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ ); 

            // Generate source/channel with sound paused
            m_Result = m_pSystem->m_pFMOD->playSound(FMOD_CHANNEL_FREE, m_Buffer, true, &m_Source);
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setChannelGroup( FMODSystem::stateStopped );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->set3DAttributes( &SourcePos, &SourceVel );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setPriority( m_iPriority );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            m_Result = m_Source->setVolume( m_fVolume );
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            
            if ( m_bListener ) 
            {
                // Play music as soon as FMOD is updated
                m_Result = m_Source->setPaused( false );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = m_Source->setChannelGroup( FMODSystem::music );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
            else if ( m_bFire || m_bWater ) 
            {
                // All sounds are created in a paused state
                // We want fire to remained paused until the fire is activated
                // We also want water to remain paused until the water is activated
                m_Result = m_Source->setChannelGroup( FMODSystem::stateStopped );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
            else if ( m_bSource && m_bLooping ) 
            {
                // Play music as soon as FMOD is updated
                m_Result = m_Source->setPaused( false );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = m_Source->setChannelGroup( FMODSystem::statePlaying );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
        } // end b_Source
    } // end if (m_pSystem->AudioInitialized())

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Object are returned in Properties
void
FMODObject::GetProperties(
    Properties::Array& Properties
    )
{
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
FMODObject::SetProperties(
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
            // Object Global Properties
            // These properties do not change for the object
            //
            if ( sName == sm_kapszPropertyNames[ Property_Listener ] )
            {
                m_bListener = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Source ] )
            {
                m_bSource = it->GetBool( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Position ] )
            {
                m_Position = it->GetVector3();
                PostChanges( System::Changes::Geometry::Position );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Orientation ] )
            {
                m_Orientation = it->GetQuaternion();
                PostChanges( System::Changes::Geometry::Orientation );
            }
            
            //
            // Object Properties
            // These are sound file dependant
            //
            else if ( sName == sm_kapszPropertyNames[ Property_SoundFile ] )
            {
                strcpy_s( m_szSoundFileName, sizeof( m_szSoundFileName ), it->GetStringPtr( 0 ) );
                
                if ( strcmp( it->GetStringPtr( 1 ), "Looping" ) == 0 )
                {
                    m_bLooping = 1;
                }
                else if ( strcmp( it->GetStringPtr( 1 ), "Random" ) == 0 )
                {
                    m_bRandom = 1;
                }
                if ( strcmp( it->GetStringPtr( 2 ), "Fire" ) == 0 )
                {
                    m_bFire = 1;
                }
                else if ( strcmp( it->GetStringPtr( 2 ), "Water" ) == 0 )
                {
                    m_bWater = 1;
                }
                if ( strcmp( it->GetStringPtr( 3 ), "CollisionOff" ) == 0 )
                {
                    m_bCollisionOff = 1;
                }
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Volume ] )
            {
                m_fVolume = it->GetFloat32( 0 );
            }
            else if ( sName == sm_kapszPropertyNames[ Property_Priority ] )
            {
                m_iPriority = it->GetInt32( 0 );
            }
            else
            {
                ASSERT( False );
            }

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns the systems changes this Object 
//                           is intrested in hearing about
System::Changes::BitMask
FMODObject::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Geometry::Position | 
           System::Changes::Geometry::Orientation |
           System::Changes::POI::Area |
           System::Changes::Input::Firehose;
}


///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)
Error
FMODObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    ASSERT( m_bInitialized );

    if (m_pSystem->AudioInitialized()) 
    {
        // Process change
        if ( ChangeType & System::Changes::Geometry::Position || 
             ChangeType & System::Changes::Geometry::Orientation ||
             ChangeType & System::Changes::Physics::Velocity )
        {
            if (!dynamic_cast<InputObject*>(pSubject)) 
            {
                m_Position = *dynamic_cast<IGeometryObject*>(pSubject)->GetPosition();
                m_Orientation = *dynamic_cast<IGeometryObject*>(pSubject)->GetOrientation();
                

                if (m_bListener)
                {
                    this->UpdateListener ( &m_Position, &m_Orientation );
                }
                if (m_bSource)
                {
                    this->UpdatePosition ( &m_Position );
                }
            }
        }

        else if( ChangeType & System::Changes::POI::Area )
        {
            IAreaObject* pAreaObject = dynamic_cast<IAreaObject*>(pSubject);
            ASSERT( pAreaObject );

            // Set data
            Math::Vector3 Min, Max;
            pAreaObject->GetAreaBB( Min, Max );
            Math::Vector3 Center = ( Min + Max ) * 0.5f;
            f32 Volume = ( Max.x - Min.x )*( Max.y - Min.y )*( Max.z - Min.z );
            
            m_FireActive = pAreaObject->IsAreaActive();
            if (m_FireActive) 
            {
                std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
                m_Result = m_Source->setPaused(false);
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = m_Source->setChannelGroup( FMODSystem::statePlaying );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }             
            
            if ( m_FireActive && Volume > 0.0 )
            {
                this->UpdatePosition( &Center );
            }
            else if ( Volume <= 0.0 )
            {// fire is gone, turn the sound off
                std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
                m_Result = m_Source->setPaused(true);
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = m_Source->setChannelGroup( FMODSystem::stateStopped );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
        }

        else if ( ChangeType & System::Changes::POI::Contact )
        {
            if (m_bCollisionOff)
            {
                std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
                m_Result = m_Source->setPaused(true);
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                m_Result = m_Source->setChannelGroup( FMODSystem::stateStopped );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            }
        }

        else if ( ChangeType & System::Changes::Input::Firehose )
        {           
            bool isPaused;

            if ( m_bWater )
            {
                std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
                m_Result = m_Source->getPaused( &isPaused );
                m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );

                // Water sound is paused, toggle it on
                if ( isPaused )
                {
                    m_Result = m_Source->setPaused(false);
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                    m_Result = m_Source->setChannelGroup( FMODSystem::statePlaying );
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                }
                // Water sound is not paused, toggle it off
                else
                {
                    m_Result = m_Source->setPaused(true);
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                    m_Result = m_Source->setChannelGroup( FMODSystem::stateStopped );
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );                                          
                }
            }   
        }
    }
    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Object
System::Changes::BitMask
FMODObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::None;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateListener - Updates the position and orientation of the listener
Error 
FMODObject::UpdateListener(
    Math::Vector3 *newPos, 
    Math::Quaternion *newOri
    )
{
    Error Err = Errors::Success;
    FMOD_VECTOR newPosition = {newPos->x, newPos->y, newPos->z};

    // Orientation
    Math::Vector3 atVec(0.0f, 0.0f, -1.0f);
    newOri->Rotate(atVec);  // rotate the atVec so adjust to the world
    FMOD_VECTOR newAt = {atVec.x, atVec.y, atVec.z};

    Math::Vector3 upVec(0.0f, 1.0f, 0.0f);
    newOri->Rotate(upVec);
    FMOD_VECTOR newUp = {upVec.x, upVec.y, upVec.z};

    std::lock_guard<std::mutex> lock(m_pSystem->m_mutex);
    // listener, position, velocity, forward, up
    m_Result = m_pSystem->m_pFMOD->set3DListenerAttributes( 0, &newPosition, 0, &newAt, &newUp );
    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
    
    return Err;
}

///////////////////////////////////////////////////////////////////////////////
// UpdatePosition - Update the position of a source object
Error 
FMODObject::UpdatePosition( 
    Math::Vector3* currPos 
    )
{
    Error Err = Errors::Success;

    FMOD_VECTOR newPosition = {currPos->x, currPos->y, currPos->z};

    // Calculate velocity in M/s
    clock_t currTime = clock();
    f32 timestep = f32( (currTime - prevTime ) / CLOCKS_PER_SEC );

    FMOD_VECTOR velocity = { 0, 0, 0 };
    
    if (timestep > 0)
    {
        velocity.x = (currPos->x - prevPos.x)/timestep * 0.01f;
        velocity.y = (currPos->y - prevPos.y)/timestep * 0.01f;
        velocity.z = (currPos->z - prevPos.z)/timestep * 0.01f;

        prevPos = *currPos;
        prevTime = currTime;
    }

    if( m_Source )
    {
        m_pSystem->m_mutex.lock();
        m_Result = m_Source->set3DAttributes( &newPosition, &velocity );
        m_pSystem->m_mutex.unlock();
        if ( m_Result != FMOD_OK) {
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
            //Debug::Print("%s\n", m_szSoundFileName );
        }
    }

    return Err;
}

///////////////////////////////////////////////////////////////////////////////
// RandomPlay - If a sound is flagged as random when properties are read in,
//              play it randomly
void 
FMODObject::RandomPlay(
    void
    )
{
    // Implementation to be completed at a later point  
}

///////////////////////////////////////////////////////////////////////////////
// LoadSound - Take a filename and check if the sound already has a buffer
//             associated.  If it does not, create a buffer, otherwise provide
//             the buffer
Error
FMODObject::LoadSound(
    char* soundFileName,
    FMOD::Sound** s
    )
{
    Error Err = Errors::Failure;
    int numSounds;
    FMOD::Sound* sound = NULL;
    char name[FILENAME_MAX];

    // Get the group which all sounds are in
    FMOD::SoundGroup* masterGroup;
    m_pSystem->m_mutex.lock();
    m_Result = m_pSystem->m_pFMOD->getMasterSoundGroup( &masterGroup );
    m_pSystem->m_mutex.unlock();
    if (m_Result != FMOD_OK) 
    {
        // remove the repeated failure check from the hot path
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
    }
    else
    {
        m_pSystem->m_mutex.lock();
        m_Result = masterGroup->getNumSounds(&numSounds);
        m_pSystem->m_mutex.unlock();
        if (m_Result != FMOD_OK) 
        {
            m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
        }
        else
        {
            // Iterate through the sounds and see if there is a match
            for (int i=0; i<numSounds; i++)
            {
                m_pSystem->m_mutex.lock();
                m_Result = masterGroup->getSound(i, &sound);
                m_pSystem->m_mutex.unlock();
                if (m_Result != FMOD_OK) 
                {
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                    continue;
                }
                m_Result = sound->getName(name, FILENAME_MAX);
                if (m_Result != FMOD_OK) 
                {
                    m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
                    continue;
                }
                if ( strcmp(soundFileName, name) == 0 )  // match found
                {
                    // Set the address of the sound
                    *s = sound;
                    // return right from here to avoid checking unnecessary conditions later
                    return Errors::Success;
                }
            }
        }
    }
    // Sound is not present, create sound
    char completePath[FILENAME_MAX] = "";
    strcpy_s(completePath, sizeof(completePath), m_pSystem->szSoundDirName.c_str());
    strcat_s(completePath, sizeof(completePath), soundFileName);
    
    m_pSystem->m_mutex.lock();
    // Creating a sound automatically adds it to the master sound group
    m_Result = m_pSystem->m_pFMOD->createSound( completePath, FMOD_3D | FMOD_SOFTWARE, NULL, &sound );
    m_pSystem->m_mutex.unlock();
    
    if (m_Result != FMOD_OK) 
    {
        m_pSystem->FMODErrorCode( m_Result, __FUNCTION__ );
    }
    else
    {
        *s = sound;
        Err = Errors::Success;
    }
    return Err;
}
