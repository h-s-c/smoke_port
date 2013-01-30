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

namespace POIType {
    /// <summary cref="POIType::POIType">
    /// POIType enum of possible POI (Point of Interest) types.
    /// </summary>
    enum POIType
    {
        e_POI_Contact,
        e_POI_Sound,
        e_POI_Fire,

        e_POI_None
    };
}


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>POI</c> POI (Point of Interest) is used to track points of interest 
///   that might affect AIs.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class POI
{
public:
    POI( void )
    {
        m_Position = Math::Vector3::Zero;
        m_Type = POIType::e_POI_None;
    }

    virtual ~POI( void ) {}

    /// <summary cref="POI::GetPosition">
    /// Returns the position of this POI.  The position is relative to the type of POI.
    /// </summary>
    /// <returns>Math::Vector3 - Returns the position of this POI</returns>
    inline Math::Vector3 GetPosition( void ) { return m_Position; }

    /// <summary cref="POI::SetPosition">
    /// Set the position of this POI.  The position is relative to the type of POI.
    /// </summary>
    /// <param name="Position">Set the position of this POI</param>
    inline void SetPosition( Math::Vector3& Position ) { m_Position = Position; }

    /// <summary cref="POI::SetPosition">
    /// Set the position of this POI.  The position is relative to the type of POI.
    /// </summary>
    /// <param name="Position">Set the position of this POI (const)</param>
    inline void SetPosition( const Math::Vector3& Position ) { m_Position = Position; }

    /// <summary cref="POI::SetPosition">
    /// Returns the type of the this POI.
    /// </summary>
    /// <returns>POIType::POIType - POI type</returns>
    /// <seealso cref="POIType::POIType"/>
    inline POIType::POIType GetType( void ) { return m_Type; }
    
    /// <summary cref="POI::SetPosition">
    /// Returns True is this POI is no longer valid.
    /// </summary>
    /// <returns>Bool - Returns if the POI has expired.</returns>
    /// <seealso cref="POIType::POIType"/>
    virtual inline Bool Expired( void ) { return True; }

protected:
    Math::Vector3    m_Position;  // Position of POI
    POIType::POIType m_Type;      // Type of POI
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>POIContact</c> POIContact (Point of Interest: Contact) is used to track 
///   contact points (i.e. collisions) that might affect AIs.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class POIContact : public POI
{
public:
    POIContact( void )
    {
        m_Position = Math::Vector3::Zero;
        m_Type = POIType::e_POI_Contact;
        m_Impact = 0.0f;
    }

    ~POIContact( void ) {}

    /// <summary cref="POIContact::GetImpact">
    /// Returns the strength of the impact that caused this contact.  
    //  The larger the number, to more significant the impact.
    /// </summary>
    /// <returns>f32 - Strength of the contact.</returns>
    inline f32  GetImpact( void ) { return m_Impact; }

    /// <summary cref="POIContact::SetImpact">
    /// Set the strength of the impact that caused this contact.  
    /// The larger the number, to more significant the impact.
    /// </summary>
    /// <param name="Impact">Strength of the contact</param>
    inline void SetImpact( f32 Impact ) { m_Impact = Impact; }

protected:
    f32 m_Impact;  // Impulse of collision
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>POISound</c> POISound (Point of Interest: Sound) is used to track 
///   sound locations that might affect AIs.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class POISound : public POI
{
public:
    POISound( void )
    {
        m_Position = Math::Vector3::Zero;
        m_Type = POIType::e_POI_Sound;
        m_Gain = 1.0f;
    }

    ~POISound( void ) {}

    /// <summary cref="POISound::GetGain">
    /// Returns the gain (volume) of the sound.  
    //  The larger the number, to more significant the sound.
    /// </summary>
    /// <returns>f32 - Gain (volume) of sound.</returns>
    inline f32  GetGain( void ) { return m_Gain; }

    /// <summary cref="POISound::SetGain">
    /// Set the gain (volume) of the sound.  
    /// The larger the number, to more significant the impact.
    /// </summary>
    /// <param name="Gain">Gain (volume) of sound</param>
    inline void SetGain( f32 Gain ) { m_Gain = Gain; }

protected:
    f32 m_Gain;  // How loud is the sound
};



///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>POIFire</c> POIFire (Point of Interest: Fire) is used to track 
///   fire locations that might affect AIs.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class POIFire : public POI
{
public:
    POIFire( void )
    {
        m_Position = Math::Vector3::Zero;
        m_Type     = POIType::e_POI_Fire;
        m_AABB_Max = Math::Vector3::Zero;
        m_AABB_Min = Math::Vector3::Zero;
        m_Name     = NULL;
    }

    ~POIFire( void ) {}

    virtual inline Bool  Expired( void ) { return False; }

    /// <summary cref="POIFire::GetAABB">
    /// Returns the AABB (Axis-Aligned Bounding Box) that encloses this fire.  
    /// </summary>
    /// <param name="Min">Set to the position that defines the min of AABB.</param>
    /// <param name="Max">Set to the position that defines the max of AABB.</param>
    inline void GetAABB( Out Math::Vector3& Min, Out Math::Vector3& Max ) 
    { 
        Min = m_AABB_Min;
        Max = m_AABB_Max;
    }

    /// <summary cref="POIFire::SetAABB">
    /// Sets the AABB (Axis-Aligned Bounding Box) that encloses this fire.  
    /// </summary>
    /// <param name="Min">Minimum position that defines the AABB.</param>
    /// <param name="Max">Maximum position that defines the AABB.</param>
    inline void SetAABB( In Math::Vector3& Min, In Math::Vector3& Max ) 
    { 
        m_AABB_Min = Min;
        m_AABB_Max = Max;
    }

    /// <summary cref="POIFire::GetName">
    /// Returns the unique name for fire POI. 
    /// The name can be used to unique define each fire.
    /// </summary>
    /// <returns>pcstr - Unique name of this fire.</returns>
    inline pcstr GetName( void ) { return m_Name; }
    
    /// <summary cref="POIFire::SetName">
    /// Sets the name for this fire.  Name must be unique for each fire.
    /// </summary>
    /// <param name="Name">Unique name of this fire.</param>
    inline void  SetName( pcstr Name ) { m_Name = Name; }

protected:
    Math::Vector3 m_AABB_Max;  // Max position of fire's AABB
    Math::Vector3 m_AABB_Min;  // Min position of fire's AABB
    pcstr         m_Name;      // Name used to ID this area
};

