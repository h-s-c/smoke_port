#pragma once

#include "Systems/Ai/Object.hpp"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>CamBot</c> Implementation of the CamBot AI; an AI that moves the camera
///   along a given path.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class CamBot : public AIObject, public ITargetObject
{
    Base::Vector3               m_POI; // look-at point
    float                       m_speed, m_AnimationScaleFactor, m_SegmentTotalTime, m_SegmentElapsedTime;
    std::vector<Base::Vector3>  m_ControlPoints;
    std::vector<Base::Vector3>  m_LookAtPoints;
    std::vector<float>          m_CPDistance;
    int                         m_CurrentCP;  // current control point
    bool                        m_bSendUpdateNotification, m_bCycleLast4Points;
    Bool                        m_bIgnore;


public:

    CamBot( ISystemScene* pSystemScene, pcstr pszName );
    virtual ~CamBot( void );

    /// <summary cref="CamBot::GetTarget">
    /// Implementation of the <c>ITargetObject::GetTarget</c> function.
    /// This function returns the Bot's current velocity
    /// </summary>
    /// <returns>Base::Vector3 - Target Position</returns>
    /// <seealso cref="ITargetObject::GetTarget"/>
    virtual Base::Vector3 GetTarget( void ) { return m_POI; }

public:
    /// <summary cref="CamBot::Update">
    /// This method is called each frame to update this the CamBot (move along the path).  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    /// <seealso cref="Bot::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="CamBot::AddControlPoint">
    /// AddControlPoint adds a position vector and look-at vector to the list of points to visit.
    /// There must be at least 4 control points as interpolation is between the middle two - the first/last one
    /// used to calculate the tangent vector. If there's no look-at point provided, the last one is reused.
    /// </summary>
    void AddControlPoint( float x1, float y1, float z1,  float x2, float y2, float z2 );
    void AddControlPoint( float x, float y, float z ); // duplicate last lookat point
    void AddControlPoint( Base::Vector3 v ) { AddControlPoint( v.x,v.y,v.z ); }
    void AddControlPoint( float x1, float y1, float z1,  Base::Vector3 v2 ) { AddControlPoint( x1,y1,z1,  v2.x,v2.y,v2.z ); }
    void AddControlPoint( Base::Vector3 v1, Base::Vector3 v2 ) { AddControlPoint( v1.x,v1.y,v1.z,v2.x,  v2.y,v2.z ); }

protected:

    bool m_bStopCambot;
    f32  m_fElapsedTime;

protected:

    /// <summary cref="CamBot::Update">
    /// SetPosition figures out which control point to use  
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    void SetPosition( f32 DeltaTime );

    /// <summary cref="CamBot::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error - any error codes</returns>
    /// <seealso cref="Bot::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );
};

