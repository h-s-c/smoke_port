#pragma once

// Base
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
// Standard Library
#include <string>


namespace POIType 
{
    // POIType enum of possible POI (Point of Interest) types.
    enum POIType
    {
        e_POI_Contact,
        e_POI_Sound,
        e_POI_Fire,

        e_POI_None
    };
}


// POI (Point of Interest) is used to track points of interest 
class POI
{
public:
    POI( void )
    {
        m_Position = Base::Vector3::Zero;
        m_Type = POIType::e_POI_None;
    }

    virtual ~POI( void ) {}

    // Returns the position of this POI.  The position is relative to the type of POI.
    inline Base::Vector3 GetPosition( void ) { return m_Position; }

    // Set the position of this POI.  The position is relative to the type of POI.
    inline void SetPosition( Base::Vector3& Position ) { m_Position = Position; }

    // Set the position of this POI.  The position is relative to the type of POI.
    inline void SetPosition( const Base::Vector3& Position ) { m_Position = Position; }

    // Returns the type of the this POI.
    inline POIType::POIType GetType( void ) { return m_Type; }
    
    // Returns True is this POI is no longer valid.
    virtual inline Bool Expired( void ) { return True; }

protected:
    Base::Vector3    m_Position;  // Position of POI
    POIType::POIType m_Type;      // Type of POI
};


// POIContact (Point of Interest: Contact) is used to track 
// contact points (i.e. collisions) that might affect stuff.
class POIContact : public POI
{
public:
    POIContact( void )
    {
        m_Position = Base::Vector3::Zero;
        m_Type = POIType::e_POI_Contact;
        m_Impact = 0.0f;
    }

    ~POIContact( void ) {}

    // Returns the strength of the impact that caused this contact.  
    // The larger the number, to more significant the impact.
    inline float  GetImpact( void ) { return m_Impact; }

    // Set the strength of the impact that caused this contact.  
    // The larger the number, to more significant the impact.
    inline void SetImpact( float impact ) { m_Impact = impact; }

protected:
    float m_Impact;  // Impulse of collision
};


// POISound (Point of Interest: Sound) is used to track sound locations that might affect stuff.
class POISound : public POI
{
public:
    POISound( void )
    {
        m_Position = Base::Vector3::Zero;
        m_Type = POIType::e_POI_Sound;
        m_Gain = 1.0f;
    }

    ~POISound( void ) {}

    // Returns the gain (volume) of the sound.  
    //  The larger the number, to more significant the sound.
    inline float  GetGain( void ) { return m_Gain; }

    // Set the gain (volume) of the sound.  
    // The larger the number, to more significant the impact.
    inline void SetGain( float gain ) { m_Gain = gain; }

protected:
    float m_Gain;  // How loud is the sound
};

// POIFire (Point of Interest: Fire) is used to track fire locations that might affect stuff.
class POIFire : public POI
{
public:
    POIFire( void )
    {
        m_Position = Base::Vector3::Zero;
        m_Type     = POIType::e_POI_Fire;
        m_AABB_Max = Base::Vector3::Zero;
        m_AABB_Min = Base::Vector3::Zero;
        m_Name     = NULL;
    }

    ~POIFire( void ) {}

    virtual inline bool  Expired( void ) { return false; }

    // Returns the AABB (Axis-Aligned Bounding Box) that encloses this fire.  
    inline void GetAABB( Out Base::Vector3& Min, Out Base::Vector3& Max ) 
    { 
        Min = m_AABB_Min;
        Max = m_AABB_Max;
    }

    // Sets the AABB (Axis-Aligned Bounding Box) that encloses this fire.  
    inline void SetAABB( In Base::Vector3& Min, In Base::Vector3& Max ) 
    { 
        m_AABB_Min = Min;
        m_AABB_Max = Max;
    }

    // Returns the unique name for fire POI. 
    // The name can be used to unique define each fire.
    inline pcstr GetName( void ) { return m_Name; }
    
    // Sets the name for this fire.  Name must be unique for each fire.
    inline void  SetName( std::string name ) { m_Name = name; }

protected:
    Base::Vector3 m_AABB_Max;  // Max position of fire's AABB
    Base::Vector3 m_AABB_Min;  // Min position of fire's AABB
    std::string   m_Name;      // Name used to ID this area
};

