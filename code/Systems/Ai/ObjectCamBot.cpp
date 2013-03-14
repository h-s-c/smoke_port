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

//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/Ai/ObjectCamBot.hpp"


template <class T>
T lerp( T v1, T v2, f32 f )
{
    T vLerp = v2;
    vLerp -= v1;
    vLerp *= f;
    vLerp += v1;
    return vLerp;
}


template <class T>
T catmullRom( T p1, T p2, T p3, T p4, f32 f )
{
    T q;
    q =          (-p1 + p2*3.0f -p3*3.0f + p4)*f*f*f
               + (p1*2.0f -p2*5.0f + p3*4.0f - p4)*f*f
               + (-p1+p3)*f
               + p2*2.0f;

    return q * 0.5f;
}


///////////////////////////////////////////////////////////////////////////////
// CamBot - Constructor
CamBot::CamBot( ISystemScene* pSystemScene, pcstr pszName ) : AIObject( pSystemScene, pszName )
{
    Base::Vector3 house(-974,625,-2855);
    Base::Vector3 coop(-3061,  93, -2309);
    Base::Vector3 truck(0,50,0);

    Base::Vector3 tree_center(-4305,1083,-4256);
    Base::Vector3 tree_side1(-4316,654,-8492);
    Base::Vector3 tree_side2(-5245,325,-1594);
    Base::Vector3 tree_mid(-7504,54,-6646);

#if 0
    AddControlPoint(Base::Vector3::Zero, house );
    AddControlPoint(coop, corral_1 ); // start location

    AddControlPoint(corral_1, corral_1_2 );   // loop around corral
    AddControlPoint(corral_1_2, corral_2 );   // loop around corral
    AddControlPoint(corral_2, corral_2_3 );   // loop around corral
    AddControlPoint(corral_2_3, corral_3 );
    AddControlPoint(corral_3, corral_3_4 ); 
    AddControlPoint(corral_3_4, corral_4 );
    AddControlPoint(corral_4, corral_4_1 ); 
    AddControlPoint(corral_4_1, corral_1 );
    AddControlPoint(corral_1, hillhalf);//, hilltop1half3 ); 

    AddControlPoint(hillhalf, hill1 );  // climb hill
    AddControlPoint(hill1, house ); 
    AddControlPoint(hill2 ); 

    AddControlPoint( corral_4 );   // move to tree


    AddControlPoint(above_house, tree_center );   // look over tree

    AddControlPoint(tree_side2 );   // look over tree
    AddControlPoint(tree_mid ); 
    AddControlPoint(tree_side1 ); 

#endif

    AddControlPoint(-3533.3621f, 0.0f,       -391.63849f, -518.98309f, 796.58795f, -2802.6040f); 

    AddControlPoint(-3534.3621f, 0.0f,       -391.63849f, -518.98309f,  796.58795f, -2802.6040f); 
    AddControlPoint( 1471.9407f, 100.0f,     -67.458618f, -1292.8783f,  851.34772f, -2935.9907f); 
    AddControlPoint( 1692.0292f, 974.18793f, -4549.7803f, -952.90619f,  607.41290f, -2977.3242f); 
    AddControlPoint(-1650.6622f, 1187.1954f, -5584.4229f, -6336.2852f, 2103.36350f,  5275.6035f); 
    AddControlPoint(-5022.9531f, 1408.2712f, -3137.8174f,  113.50073f,  722.45496f,  1362.4749f);
    AddControlPoint(-3305.0298f, 1217.0206f,  3038.8462f);
    AddControlPoint( 1993.2080f, 795.25165f,  3830.2681f);
    AddControlPoint( 6015.0225f, 709.07837f,  3583.7188f);
    AddControlPoint( 5676.4614f, 709.07837f,  1787.2061f, -690.02954f,  722.45496f,  1362.4749f); 
    AddControlPoint( 839.61084f, 709.07837f,  1787.2061f, -6156.4648f,  722.45496f,  1216.1666f); 
    AddControlPoint(-6903.0767f, 1283.51569f,  2938.7991f, -4279.6162f,  722.45496f, -2125.4819f); 
    AddControlPoint(-6236.3472f, 142.01349f,  327.10275f, -2154.5854f,  851.34772f, -2620.8501f); 
    AddControlPoint(-3534.3621f, 0.0f,       -391.63849f, -518.98309f,  796.58795f, -2802.604f ); 

    AddControlPoint( 1471.9407f, 100.0f,     -67.458618f, -1292.8783f,  851.34772f, -2935.9907f);
    AddControlPoint( 1692.0292f, 974.18793f, -4549.7803f, -952.90619f,  607.41290f, -2977.3242f);
    AddControlPoint(-1650.6622f, 1187.1954f, -5584.4229f, -6336.2852f, 2103.36350f,  5275.6035f);
    AddControlPoint(-4823.9531f, 1408.2712f, -1937.8174f, -6597.9727f, 2103.36350f,  5838.1113f); 

    m_AnimationScaleFactor  = 1.0f;
    m_speed  = 1000.0f;
    m_bCycleLast4Points  = true; // if false halts when reaches end

    m_CurrentCP = 1;
    m_SegmentElapsedTime = 0.0f;
    m_SegmentTotalTime = m_CPDistance[m_CurrentCP]/m_speed;
    m_bSendUpdateNotification = false;

    m_bStopCambot = false;
    m_fElapsedTime = 0.0f;
    m_bIgnore = True;
   
}


///////////////////////////////////////////////////////////////////////////////
// ~CamBot - Destructor
CamBot::~CamBot( void )
{
}

///////////////////////////////////////////////////////////////////////////////
void CamBot::AddControlPoint( float x, float y, float z )
{
    ASSERT( m_LookAtPoints.size() > 0 );
    Base::Vector3 la( m_LookAtPoints[m_LookAtPoints.size()-1] );
    AddControlPoint(x,y,z,la.x,la.y,la.z );
}

///////////////////////////////////////////////////////////////////////////////
void CamBot::AddControlPoint( float x1, float y1, float z1, float x2, float y2, float z2  )
{
    m_ControlPoints.push_back(Base::Vector3(x1,y1,z1));
    m_LookAtPoints.push_back(Base::Vector3(x2,y2,z2));

    // if there's more than 2 control points, figure out the distance from
    // this latest one to the last one
    size_t idx = m_ControlPoints.size();
    if ( m_ControlPoints.size() > 1 )
    {
      m_CPDistance.push_back( (m_ControlPoints[idx-1] - m_ControlPoints[idx-2]).Magnitude() );
    }

}


///////////////////////////////////////////////////////////////////////////////
// ~CamBot - Destructor
void CamBot::SetPosition( f32 DeltaTime )
{

  //PostChanges(System::Changes::Geometry::Position);
  //Debug::Print("cameraLoc = %5.2f,%5.2f,%5.2f\n",m_Position.x,m_Position.y,m_Position.z);
  //return;

  // if we've gone beyond last segment, which thing to do?
  if ( m_bCycleLast4Points && (unsigned)m_CurrentCP >= m_ControlPoints.size()-2 )
  {
    m_CurrentCP -= 13; // replay last 4
    m_SegmentTotalTime = 0;
  }
  else if ( (unsigned)m_CurrentCP >= m_ControlPoints.size()-2 )
  {
    //Debug::Print("Past last control point\n");
    //Debug::Print("cameraLoc = %5.2f,%5.2f,%5.2f    POI=%5.2f,%5.2f,%5.2f\n",m_Position.x,m_Position.y,m_Position.z,m_POI.x,m_POI.y,m_POI.z);
    return; 
  }


  m_SegmentElapsedTime += DeltaTime;

  // if we've gone beyond this segment, move to next one
  if ( m_SegmentElapsedTime > m_SegmentTotalTime )
  {
    ++m_CurrentCP;
    m_SegmentElapsedTime = 0.0f;
    m_SegmentTotalTime = m_CPDistance[m_CurrentCP-1]/m_speed;
    //Debug::Print("Moving to next control point - #%d to #%d   it will take %f seconds for this segment of %f @ %fcm/s\n",m_CurrentCP,m_CurrentCP+1, m_SegmentTotalTime, m_CPDistance[m_CurrentCP],m_CPDistance[m_CurrentCP]/m_SegmentTotalTime);
    SetPosition( DeltaTime );
    return;
  }

  float f = Base::Min( 1.0f, m_SegmentElapsedTime/m_SegmentTotalTime );

  //Debug::Print("Fraction of segment = %f  elapsed Distance = %f\n",f,m_SegmentElapsedTime*m_speed);

  //m_Position = lerp(m_ControlPoints[m_CurrentCP], m_ControlPoints[m_CurrentCP+1], f);
  //m_POI      = lerp(m_LookAtPoints[m_CurrentCP], m_LookAtPoints[m_CurrentCP+1], f);
  m_Position = catmullRom(m_ControlPoints[m_CurrentCP-1], m_ControlPoints[m_CurrentCP],m_ControlPoints[m_CurrentCP+1], m_ControlPoints[m_CurrentCP+2], f );
  m_POI      = catmullRom(m_LookAtPoints[m_CurrentCP-1], m_LookAtPoints[m_CurrentCP],m_LookAtPoints[m_CurrentCP+1], m_LookAtPoints[m_CurrentCP+2], f );
  m_bSendUpdateNotification = true;

}

///////////////////////////////////////////////////////////////////////////////
// ChangeOccurred - Notify this Object a change occureed (change handler)
Error CamBot::ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType )
{
    ASSERT( m_bInitialized );

    if ( dynamic_cast<IInputObject*>(pSubject) )
    {
        if ( ChangeType & (System::Changes::Geometry::Position | System::Changes::Geometry::Orientation) ) 
        {
            if ( !m_bIgnore )
            {
                m_bSendUpdateNotification = false;
                m_bStopCambot = true;
                m_fElapsedTime = 0.0f;
                return Errors::Success;
            }
            else
            {
                m_bIgnore = False;
            }
        }
    }
    
    return Errors::Success;
}

///////////////////////////////////////////////////////////////////////////////
// Update - Main update
void CamBot::Update( f32 DeltaTime )
{

  if ( !m_bStopCambot )
  {
    SetPosition(DeltaTime);
  } 
  else
  {
    m_fElapsedTime += DeltaTime;
    if (m_fElapsedTime >= 30.0f)
    {
        m_bStopCambot = false;
    }
  }

  if ( m_bSendUpdateNotification )
  {

    PostChanges(System::Changes::Geometry::Position | System::Changes::POI::Target);
//  PostChanges(System::Changes::Geometry::Orientation);
    //Debug::Print("cameraLoc = %5.2f,%5.2f,%5.2f    POI=%5.2f,%5.2f,%5.2f\n",m_Position.x,m_Position.y,m_Position.z,m_POI.x,m_POI.y,m_POI.z);
    m_bSendUpdateNotification = false;
    //Debug::Print("cameraOr = %5.2f,%5.2f,%5.2f,%5.2f\n",m_Orientation.x,m_Orientation.y,m_Orientation.z,m_Orientation.w);
  }

}
