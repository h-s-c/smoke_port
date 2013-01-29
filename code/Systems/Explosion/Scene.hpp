#pragma once

//internal
#include "Systems/Explosion/POI.hpp"
//stdlib
#include <mutex>

class ExplosionSystem;
class ExplosionTask;
class ExplosionObject;

/// <summary>
/// ExplosionScene class: Implementation of the ISystemScene interface. 
/// </summary>
class ExplosionScene : public ISystemScene
{
    friend class ExplosionSystem;
    friend class ExplosionTask;

public:
    inline std::vector<ExplosionObject*> GetObjects( void ) { return m_Objects; }
    inline std::list<POI*> GetPOI( void ) { return m_POI; }

    void PostUpdate( void );

protected:

    ExplosionScene( ISystem* pSystem );
    ~ExplosionScene( void );

    /// <summary cref="ISystemScene::GetSystemType">
    ///   Implementation of the ISystemScene GetSystemType function.
    /// </summary>
    virtual System::Type GetSystemType( void );

    virtual void Update( f32 DeltaTime );

    virtual Error Initialize( std::vector<Properties::Property> Properties );

    virtual void GetProperties( Properties::Array& Properties );
    virtual void SetProperties( Properties::Array Properties );

    virtual pcstr* GetObjectTypes( void );
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    virtual ISystemTask* GetSystemTask( void );

    virtual System::Changes::BitMask GetPotentialSystemChanges( void );
    virtual System::Changes::BitMask GetDesiredSystemChanges( void );

    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

protected:

    ExplosionTask*                 m_pExplosionTask;   // Main task for this scene
    std::vector<ExplosionObject*>  m_Objects;          // Scene objects
    std::list<POI*>         m_POI;                     // Scene points of interest
    std::mutex m_mutex;
};

