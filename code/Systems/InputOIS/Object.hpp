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


class InputSystem;
class InputScene;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implementation of the ISystemObject interface.
///   See Interfaces\System.h for a definition of the class and its functions.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class InputObject : public ISystemObject, public IGeometryObject, public IGUIObject, public IInputObject
{
    friend class InputScene;
    friend class InputTask;

protected:

    InputObject( ISystemScene* pSystemScene );
    InputObject( ISystemScene* pSystemScene, pcstr pszType, pcstr pszName);
    virtual ~InputObject( void );

    /// <summary cref="InputObject::GetSystemType">
    ///   Implementation of the <c>ISystemObject::GetSystemType</c> function.
    ///   Lets this object know when a registered aspects of interest has changed 
    ///   (this function will be called when other systems make changes this object should know about).
    /// </summary>
    /// <returns>System::Type - Type of this system.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="InputObject::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the input object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="InputObject::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the input object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    void GetProperties( Properties::Array& Properties );

    /// <summary cref="InputObject::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the sound object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    void SetProperties( Properties::Array Properties );

    /// <summary cref="InputObject::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the sound object.</returns>
    /// <seealso cref="ISystemObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="InputObject::ChangeOccurred">
    ///   Lets this object know when a registered aspects of interest has changed 
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="IObserver::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="InputObject::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this sound.</returns>
    /// <seealso cref="ISubject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

public:
    /// <summary cref="IGeometryObject::GetPosition">
    ///   Implementation of the IGeometryObject GetPosition function.
    /// </summary>
    virtual const Base::Vector3* GetPosition( void );

public:
    /// <summary cref="IGeometryObject::GetOrientation">
    ///   Implementation of the IGeometryObject GetOrientation function.
    /// </summary>
    virtual const Base::Quaternion* GetOrientation( void );
protected:
    /// <summary cref="IGeometryObject::GetScale">
    ///   Implementation of the IGeometryObject GetScale function.
    /// </summary>
    virtual const Base::Vector3* GetScale( void );

    /// <summary cref="IGUIObject::GetWindowData">
    ///   Implementation of the IGUIObject GetWindowData function.
    /// </summary>
    virtual const WindowData* GetWindowData( void );

protected:

    Base::Vector3                       m_Position;

    // NOTE: m_Orientation is not really storing a Quaternion.  The data structure is being
    //       used to store pitch and yaw information.
    Base::Quaternion                    m_Orientation;

    f32                                 m_Yaw;
    f32                                 m_Pitch;
    f32                                 m_Roll;

    static pcstr                        sm_kapszTypeNames[];

    enum Types
    {
        Type_Controlled, Type_GUI
    };
    Types                               m_Type;
    bool                                m_bVisible;

    

    WindowData                          m_LayoutData;

    std::string                         m_sName;
    int                                 m_nFunctionKey;

    enum PropertyTypes
    {
        Property_FKey,
        Property_Orientation,
        Property_Count,
        Property_Instrumentation
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];
};

