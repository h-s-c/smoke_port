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


class HavokPhysicsSystem;
class HavokPhysicsScene;
class HavokPhysicsTask;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>HavokObject</c> Implementation of the ISystemObject interface.  
///   This is the base object created by the HavokPhysics Scene.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class HavokObject : public ISystemObject, public IGeometryObject
{
    friend HavokPhysicsSystem;
    friend HavokPhysicsScene;
    friend HavokPhysicsTask;

protected:

    HavokObject( ISystemScene* pSystemScene, pcstr pszName );
    ~HavokObject( void );

    /// <summary cref="HavokObject::SetType">
    ///   Sets the type for this object (Physics or Character)
    /// </summary>
    /// <param name="pszType">Type of this object</param>
    inline void SetType( pcstr pszType )
    {
        m_sType = pszType;
    }

    /// <summary cref="HavokObject::GetType">
    ///   Gets the type for this object (Physics or Character)
    /// </summary>
    /// <returns>pcstr - Type of this object</returns>
    inline pcstr GetType( void )
    {
        return m_sType.c_str();
    }

    /// <summary cref="HavokObject::GetSystemType">
    ///   Implementation of the <c>ISystemObject::GetSystemType</c> function.
    ///   Lets this object know when a registered aspects of interest has changed 
    ///   (this function will be called when other systems make changes this object should know about).
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Type GetSystemType( void );

	/////////////////////////////////
	/// IGeometryObject overrides
	/////////////////////////////////

    /// <summary cref="HavokObject::GetPosition">
    ///   Implementation of the <c>IGeometryObject::GetPosition</c> function.
    /// </summary>
    /// <returns>Math::Vector3* - Returns the position for this object.</returns>
    /// <seealso cref="IGeometryObject::GetPosition"/>
	virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="HavokObject::GetOrientation">
    ///   Implementation of the <c>IGeometryObject::GetOrientation</c> function.
    /// </summary>
    /// <returns>Math::Quaternion* - Returns the orientation quaternion for this object.</returns>
    /// <seealso cref="IGeometryObject::GetOrientation"/>
	virtual const Base::Quaternion* GetOrientation( void );

    /// <summary cref="HavokObject::GetScale">
    ///   Implementation of the <c>IGeometryObject::GetScale</c> function.
    /// </summary>
    /// <returns>Math::Vector3* - Returns the scale for this object.</returns>
    /// <seealso cref="IGeometryObject::GetScale"/>
	virtual const Base::Vector3* GetScale( void );

    /// <summary cref="HavokObject::Update">
    /// Called by the task to have the object update itself.
    /// </summary>
    /// <param name="DeltaTime">Elapsed time since the last frame.</param>
    virtual void Update( f32 DeltaTime = 0.0f ) = 0;

protected:
    Base::Vector3    m_Position;
    Base::Quaternion m_Orientation;
    Base::Vector3    m_Scale;

    std::string      m_sType;
};

