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


#pragma once

//
// forward declarations
//
class AISystem;
class AIScene;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>AIState</c> Used to track and trigger AI state changes.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class AIState
{
#define STATEFLAG_NEW 0x00000001

public:
    AIState( void ) 
    { 
        m_State = -1;
        m_PreviousState = -1;
        m_Flags = 0;
        m_Duration = 0.0f; 
    }

    ~AIState( void ) {}

    /// <summary cref="AIState::SetState">
    /// Sets the current state.  
    /// This will reset duration and set the previous state.  
    /// If the previous state that State are different, this counts as "Triggering" a state change.
    /// </summary>
    /// <param name="State">The desired state.</param>
    inline void SetState( u32 State )
    {
        m_PreviousState = m_State;
        m_State = State;
        if( m_PreviousState != m_State )
        {
            m_Flags |= STATEFLAG_NEW;
        }
        m_Duration = 0.0f;
    }

    /// <summary cref="AIState::Triggered">
    /// Returns True if the state has changed since Tiggered was last changed.
    /// </summary>
    /// <returns>Bool - Recent state change</returns>
    inline Bool Triggered( void )
    {
        if( m_Flags & STATEFLAG_NEW )
        {
            m_Flags &= ~STATEFLAG_NEW;
            return True;
        }

        return False;
    }

    /// <summary cref="AIState::InitState">
    /// Sets the state.  
    /// This will overwrite current and previous the state.  
    /// This does not counts as "Triggering" a state change.
    /// </summary>
    /// <param name="State">The desired state.</param>
    inline void InitState( i32 State ) { m_State = m_PreviousState = State; }
    
    /// <summary cref="AIState::GetState">
    /// Returns the current state.
    /// </summary>
    /// <returns>i32</returns>
    inline i32 GetState( void ) { return m_State; }
    
    /// <summary cref="AIState::GetPreviousState">
    /// Returns the previous state. 
    /// </summary>
    /// <returns>i32 - previous state</returns>
    inline i32 GetPreviousState( void ) { return m_PreviousState; }
    
    /// <summary cref="AIState::GetFlags">
    /// Returns internal flags.  You should not have to use this function.
    /// </summary>
    /// <returns>i32 - internal flags</returns>
    inline i32 GetFlags( void ) { return m_Flags; }
    
    /// <summary cref="AIState::GetTime">
    /// Returns the duration of the current state.
    /// </summary>
    /// <returns>f32 - time in seconds</returns>
    inline f32 GetTime( void ) { return m_Duration; }
    
    /// <summary cref="AIState::UpdateTime">
    /// Updates the internal timer.
    /// </summary>
    /// <param name="DeltaTime">Time since UpdateTime was last called (in seconds).</param>
    /// <seealso cref="AIState::GetTime"/>
    inline void UpdateTime( f32 DeltaTime ) { m_Duration += DeltaTime; }

private:
    i32 m_State;          // Current state
    i32 m_PreviousState;  // Previous state
    i32 m_Flags;          // Flags about this state
    f32 m_Duration;       // Time in the current state
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>AIObject</c> Implementation of the ISystemObject interface.  
///   This is the base object created by the AI Scene.  
///   One object is created per AI.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class AIObject : public ISystemObject, public IGeometryObject, public IBehaviorObject
{
    friend class AIScene;
    friend class AITask;

protected:

    AIObject( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~AIObject( void );

    /// <summary cref="AIObject::GetSystemType">
    ///   Implementation of the <c>ISystemObject::GetSystemType</c> function.
    ///   Lets this object know when a registered aspects of interest has changed 
    ///   (this function will be called when other systems make changes this object should know about).
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="AIObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the AI object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="AIObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the AI object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="AIObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the AI object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="AIObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the AI object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="AIObject::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="AIObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this AI.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="AIObject::GetPosition">
    ///   Implementation of the <c>IGeometryObject::GetPosition</c> function.
    /// </summary>
    /// <returns>Base::Vector3* - Returns the position for this AI object.</returns>
    /// <seealso cref="IGeometryObject::GetPosition"/>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="AIObject::GetOrientation">
    ///   Implementation of the <c>IGeometryObject::GetOrientation</c> function.
    /// </summary>
    /// <returns>Base::Quaternion* - Returns the orientation quaternion for this AI object.</returns>
    /// <seealso cref="IGeometryObject::GetOrientation"/>
    virtual const Base::Quaternion* GetOrientation( void );

    /// <summary cref="AIObject::GetScale">
    ///   Implementation of the <c>IGeometryObject::GetScale</c> function.
    /// </summary>
    /// <returns>Base::Vector3* - Returns the scale for this AI object.</returns>
    /// <seealso cref="IGeometryObject::GetScale"/>
    virtual const Base::Vector3* GetScale( void );


    /// <summary cref="AIObject::GetBehavior">
    ///   Implementation of the <c>IBehaviorObject::GetBehavior</c> function.
    /// </summary>
    /// <returns>Returns the current behavior of this AI object.</returns>
    /// <seealso cref="IBehaviorObject::GetScale"/>
    virtual Behavior GetBehavior();

public:
    /// <summary cref="AIObject::PreUpdate">
    /// This method is called before each <c>Update</c> call to perform pre-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PreUpdate( f32 DeltaTime );

    /// <summary cref="AIObject::Update">
    /// Implementation of the <c>ISystemTask::Update</c> method.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="AIObject::PostUpdate">
    /// This method is called after each <c>Update</c> call to perform post-processing.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void PostUpdate( f32 DeltaTime );

    /// <summary cref="AIObject::GetState">
    ///   Returns the state of the AI object.
    /// </summary>
    /// <returns>i32 - Returns the state for this AI.</returns>
    inline i32 GetState( void ) { return m_State.GetState(); }

    Base::Vector3       m_Position;     // Position of AI object
    Base::Quaternion    m_Orientation;  // Orientation of AI object
    Base::Vector3       m_Scale;        // Scale of AI object

protected:
    /// <summary cref="AIObject::SetBehavior">
    ///   Set the current behavior for this AI object.  
    ///   This will notify other systems of the changed behavior.
    /// </summary>
    /// <param name="Behavior">The desired behavior.</param>
    void SetBehavior( Interface::Behavior Behavior );

    AIState             m_State;        // State of AI (internal use)
    Interface::Behavior m_Behavior;     // Behavior of AI (for external referece by other systems)
};

