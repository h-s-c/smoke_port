#pragma once
// System
#include "Systems/Explosion/Object.hpp"

// Foward declarations
/// <summary>
/// ExplosionType enum of possible explosions.
/// </summary>
namespace ExplosionType 
{
    enum ExplosionType
    {
        e_None,
        e_MeteorImpact,
    };
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>Explosion</c> Base class for explosion objects.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class Explosion : public ExplosionObject, public IMoveObject
{
public:

    /// <summary cref="Explosion::Explosion">
    /// ctor
    /// </summary>
    /// <param name="pSystemScene">Pointer to the <c>ISystemScene</c> object.</param>
    /// <param name="pszName">Name of the explosion object.</param>
    Explosion( ISystemScene* pSystemScene, pcstr pszName );

    /// <summary cref="Explosion::~Explosion">
    /// dtor
    /// </summary>
    virtual ~Explosion( void );

    /// <summary cref="Explosion::ChangeOccurred">
    /// Implementation of the <c>IObserver::ChangeOccurred</c> method.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error
    /// </returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );
    
    /// <summary cref="Explosion::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PreUpdate( f32 DeltaTime );
   
    /// <summary cref="Explosion::ChangeOccurred">
    /// Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );
    
    /// <summary cref="Explosion::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PostUpdate( f32 DeltaTime );

    /// <summary cref="Explosion::GetVelocity">
    /// Implementation of the <c>IMoveObject::GetVelocity</c> method.
    /// </summary>
    /// <returns>Velocity of the object.
    /// </returns>
    /// <seealso cref="IMoveObject::GetVelocity"/>
    virtual const Base::Vector3* GetVelocity();

    /// <summary cref="Explosion::GetPosition">
    /// Implementation of the <c>IMoveObject::GetPosition</c> method.
    /// </summary>
    /// <returns>Position of the object.
    /// </returns>
    /// <seealso cref="IMoveObject::GetPosition"/>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="Explosion::GetMaxVelocity">
    /// Implementation of the <c>IMoveObject::GetMaxVelocity</c> method.
    /// </summary>
    /// <returns>Magnitude of the velocity of the object.
    /// </returns>
    /// <seealso cref="IMoveObject::GetMaxVelocity"/>
    virtual f32 GetMaxVelocity() { return m_Velocity.Magnitude(); }

protected:

    /// <summary cref="Explosion::GetPotentialSystemChanges">
    /// Implementation of the <c>ISubject::GetPotentialSystemChanges</c> method.
    /// </summary>
    /// <returns>Returns systems changes possible for this Explosion
    /// </returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="Explosion::GetProperties">
    /// Implementation of the <c>ISystem::GetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Get properties for this Explosion object.</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="Explosion::SetProperties">
    /// Implementation of the <c>ISystem::SetProperties</c> method.
    /// </summary>
    /// <param name="Properties">Set properties for this Explosion object.</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

public:

    /// <summary>
    /// Explosion type
    /// </summary>
    ExplosionType::ExplosionType m_Type;      // Type of Explosion

    /// <summary>
    /// Reserved: Initial force exerted by the explosion
    /// </summary>
    f32     m_BlastForce;   // How much force the explosion exerts radially
    /// <summary>
    /// Reserved: Blast radius for the explosion
    /// </summary>
    f32     m_BlastRadius;  // Radius of sphere of influence for the blast effect

    /// <summary cref="Explosion::SetVelocity">
    /// Sets the velocity for the Explosion object.
    /// </summary>
    /// <param name="velocity">Velocity of the object.</param>
    void SetVelocity(Base::Vector3 velocity);

private:

    /// <summary >
    /// Explosion system PropertyTypes enum
    /// </summary >
    enum PropertyTypes
    {
        Property_Force,     // Force delivered to surrounding entities
        Property_Radius,    // Area of effect for the blast defined by this radius of the bounding sphere
        Property_Count
    };

    /// <summary >
    /// Explosion velocity vector
    /// </summary >
    Base::Vector3 m_Velocity; // Velocity vector describing the explosive effect on other entities

    /// <summary >
    /// Explosion system Property name array
    /// </summary >
    static pcstr sm_kapszPropertyNames[];
    /// <summary >
    /// Explosion system Property array of default values
    /// </summary >
    static const Properties::Property sm_kaDefaultProperties[]; 
};

