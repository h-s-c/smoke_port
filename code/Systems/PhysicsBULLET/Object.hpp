/* The MIT License (MIT)
 * Copyright (c) 2013 Kevin Schmidt
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once

class BulletPhysicsSystem;
class BulletPhysicsScene;
class BulletPhysicsTask;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>BulletObject</c> Implementation of the ISystemObject interface.  
///   This is the base object created by the BulletPhysics Scene.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class BulletObject : public ISystemObject, public IGeometryObject
{
    friend BulletPhysicsSystem;
    friend BulletPhysicsScene;
    friend BulletPhysicsTask;

protected:

    BulletObject( ISystemScene* pSystemScene, pcstr pszName );
    ~BulletObject( void );

    /// <summary cref="BulletObject::SetType">
    ///   Sets the type for this object (Physics or Character)
    /// </summary>
    /// <param name="pszType">Type of this object</param>
    inline void SetType( pcstr pszType )
    {
        m_sType = pszType;
    }

    /// <summary cref="BulletObject::GetType">
    ///   Gets the type for this object (Physics or Character)
    /// </summary>
    /// <returns>pcstr - Type of this object</returns>
    inline pcstr GetType( void )
    {
        return m_sType.c_str();
    }

    /// <summary cref="BulletObject::GetSystemType">
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

    /// <summary cref="BulletObject::GetPosition">
    ///   Implementation of the <c>IGeometryObject::GetPosition</c> function.
    /// </summary>
    /// <returns>Base::Vector3* - Returns the position for this object.</returns>
    /// <seealso cref="IGeometryObject::GetPosition"/>
    virtual const Base::Vector3* GetPosition( void );

    /// <summary cref="BulletObject::GetOrientation">
    ///   Implementation of the <c>IGeometryObject::GetOrientation</c> function.
    /// </summary>
    /// <returns>Base::Quaternion* - Returns the orientation quaternion for this object.</returns>
    /// <seealso cref="IGeometryObject::GetOrientation"/>
    virtual const Base::Quaternion* GetOrientation( void );

    /// <summary cref="BulletObject::GetScale">
    ///   Implementation of the <c>IGeometryObject::GetScale</c> function.
    /// </summary>
    /// <returns>Base::Vector3* - Returns the scale for this object.</returns>
    /// <seealso cref="IGeometryObject::GetScale"/>
    virtual const Base::Vector3* GetScale( void );

    /// <summary cref="BulletObject::Update">
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

