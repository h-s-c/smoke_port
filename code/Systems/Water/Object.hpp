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
class WaterSystem;
class WaterScene;

namespace WaterType {
    /// <summary>
    /// <c>WaterType</c> Enum of possible explosions.
    /// </summary>
    enum WaterType
    {
        e_None,
        e_WaterStream,
    };
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>WaterState</c> WaterState is used to track and trigger Water state changes.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
class WaterState
{
#define STATEFLAG_NEW 0x00000001

public:
    WaterState( void ) 
    { 
        m_State = -1;
        m_PreviousState = -1;
        m_Flags = 0;
        m_Duration = 0.0f; 
    }

    ~WaterState( void ) {}

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

    inline bool Triggered( void )
    {
        if( m_Flags & STATEFLAG_NEW )
        {
            m_Flags &= ~STATEFLAG_NEW;
            return True;
        }

        return False;
    }

    inline void InitState( i32 State ) { m_State = m_PreviousState = State; }
    inline i32 GetState( void ) { return m_State; }
    inline i32 GetPreviousState( void ) { return m_PreviousState; }
    inline i32 GetFlags( void ) { return m_Flags; }
    inline f32 GetTime( void ) { return m_Duration; }
    inline void UpdateTime( f32 DeltaTime ) { m_Duration += DeltaTime; }

private:
    i32 m_State;          // Current state
    i32 m_PreviousState;  // Previous state
    i32 m_Flags;          // Flags about this state
    f32 m_Duration;       // Time in the current state
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>WaterObject</c> Implementation of the ISystemObject interface.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////
class WaterObject : public ISystemObject, public IGeometryObject
{
    friend class WaterScene;
    friend class WaterTask;

protected:

    WaterObject( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~WaterObject( void );

    /// <summary cref="ISystemObject::GetSystemType">
    ///   Implementation of the ISystemObject GetSystemType function.
    /// </summary>
    virtual System::Type GetSystemType( void );

    virtual Error Initialize( std::vector<Properties::Property> Properties );

    virtual void GetProperties( Properties::Array& Properties );
    virtual void SetProperties( Properties::Array Properties );

    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="IObserver::ChangeOccurred">
    ///   Implementation of the IObserver ChangeOccurred function.
    /// </summary>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="ISubject::GetPotentialSystemChanges">
    ///   Implementation of the ISubject GetPotentialSystemChanges function.
    /// </summary>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="IGeometryObject::GetPosition">
    ///   Implementation of the IGeometryObject GetPosition function.
    /// </summary>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="IGeometryObject::GetOrientation">
    ///   Implementation of the IGeometryObject GetOrientation function.
    /// </summary>
    virtual const Base::Quaternion* GetOrientation( void );

    /// <summary cref="IGeometryObject::GetScale">
    ///   Implementation of the IGeometryObject GetScale function.
    /// </summary>
    virtual const Base::Vector3* GetScale( void );

public:
    virtual void Update( f32 DeltaTime/*, std::vector<WaterObject*> WaterObjectList*/ );

    inline i32 GetState( void ) { return m_State.GetState(); }

    Base::Vector3       m_Position;     // Position of Water object
    Base::Quaternion    m_Orientation;  // Orientation of Water object
    Base::Vector3       m_Scale;        // Scale of Water object

    enum Types
    {
        Type_WaterStream, Type_Unknown
    };
    
    Types               m_Type;

protected:
   
    WaterState          m_State;        // State of Water (internal use)
};

