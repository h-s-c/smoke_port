//internal
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/AudioFMOD/Object.hpp"
#include "Systems/AudioFMOD/ObjectAI.hpp"
#include "Systems/AudioFMOD/ObjectCollision.hpp"
#include "Systems/AudioFMOD/Scene.hpp"
#include "Systems/AudioFMOD/System.hpp"
#include "Systems/AudioFMOD/Task.hpp"
//external
#include "fmod.hpp"

///////////////////////////////////////////////////////////////////////////////
// FMODScene - Constructor
FMODScene::FMODScene(
    ISystem* pSystem
    )
    : ISystemScene( pSystem )
    , m_pFMODTask( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// FMODScene - Destructor
FMODScene::~FMODScene(
    void
    )
{
    SAFE_DELETE( m_pFMODTask );

    //
    // Free all the remaining objects.
    //
    for ( std::list<FMODObject*>::iterator it=m_Objects.begin();
          it != m_Objects.end(); it++ )
    {
        delete *it;
    }

    m_Objects.clear();
}

///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this Scene
System::Type
FMODScene::GetSystemType(
    void
    )
{
    return System::Types::Audio;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this Scene with the given properties
Error
FMODScene::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    ASSERT( !m_bInitialized );

    // Create a new FMOD Task
    m_pFMODTask = new FMODTask( this );
    ASSERT( m_pFMODTask != NULL );

    // Initialized
    m_bInitialized = True;

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this Scene are returned in Properties
void
FMODScene::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}

///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this Scene
void
FMODScene::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}

///////////////////////////////////////////////////////////////////////////////
// GetObjectTypes - Get Object types for this Scene
pcstr*
FMODScene::GetObjectTypes(
    void
    )
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CreateObject - Create an Object with the given Name and Type
ISystemObject*
FMODScene::CreateObject(
    pcstr pszName,
    pcstr pszType
    )
{
    UNREFERENCED_PARAM( pszName );

    // Create the FMOD object
    FMODObject* pObject = NULL;
    if( pszType == NULL )
    {
        pObject = (FMODObject*)new FMODObject( this );
    }
    else if( strcmp( pszType, "AI" ) == 0 )
    {
        pObject = (FMODObject*)new FMODObjectAI( this );
    }
    else if( strcmp( pszType, "Collision" ) == 0 )
    {
        pObject = (FMODObject*)new FMODObjectCollision( this );
    }

    if ( pObject != NULL )
    {
        m_Objects.push_back( pObject );
    }

    return pObject;
}

///////////////////////////////////////////////////////////////////////////////
// DestroyObject - Destorys the given Object, removing it from the Scene
Error
FMODScene::DestroyObject(
    ISystemObject* pSystemObject
    )
{
    ASSERT( pSystemObject != NULL );

    //
    // Cast to a GeometryObject so that the correct destructor will be called.
    //
    FMODObject* pObject = reinterpret_cast<FMODObject*>(pSystemObject);

    //
    // Remove the object from the list and delete it.
    //
    m_Objects.remove( pObject );

    SAFE_DELETE( pSystemObject );

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// GetSystemTask - Returns the task associated with this Scene
ISystemTask*
FMODScene::GetSystemTask(
    void
    )
{
    return m_pFMODTask;
}

///////////////////////////////////////////////////////////////////////////////
// GetPotentialSystemChanges - Returns systems changes possible for this Scene
System::Changes::BitMask
FMODScene::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::None;
}

///////////////////////////////////////////////////////////////////////////////
// GetDesiredSystemChanges - Returns systems this Scene is interested in
System::Changes::BitMask 
FMODScene::GetDesiredSystemChanges( 
    void 
    )
{
    return System::Changes::POI::Contact;
}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Process changes that occured by other systems/objects
Error 
FMODScene::ChangeOccurred( 
    ISubject* pSubject, 
    System::Changes::BitMask ChangeType 
    )
{
    if ( ChangeType & System::Changes::POI::Contact )
    {
        IContactObject* pContactObject = dynamic_cast<IContactObject*>(pSubject);
        ASSERT( pContactObject );

        const IContactObject::Info* pContactInfo = pContactObject->GetContact();
        UNREFERENCED_PARAM( pContactInfo );
    }

    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// Update - Main Update for the FMOD Scene
void FMODScene::Update(f32 DeltaTime)
{
    UNREFERENCED_PARAM( DeltaTime );
    std::list<FMODObject*>::iterator it = m_Objects.begin();
    for ( it = m_Objects.begin(); it != m_Objects.end(); it++ )
    {
        FMODObject* pObject = *it;

        pObject->RandomPlay();
    }
}
