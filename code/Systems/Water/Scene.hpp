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

class WaterSystem;
class WaterTask;
class WaterObject;

/*******************************************************************************
* CLASS: WaterScene
*
* DESCRIPTION:
* Implementation of the ISystemScene interface.
* See Interfaces\System.h for a definition of the class and its functions.
*******************************************************************************/

class WaterScene : public ISystemScene
{
    friend class WaterSystem;
    friend class WaterTask;

public:
    inline std::vector<WaterObject*> GetObjects( void ) { return m_Objects; }

protected:

    WaterScene( ISystem* pSystem );
    ~WaterScene( void );

    /// <summary cref="ISystemScene::GetSystemType">
    ///   Implementation of the ISystemScene GetSystemType function.
    /// </summary>
    virtual System::Type GetSystemType( void );

    virtual Error Initialize( std::vector<Properties::Property> Properties );

    virtual void GetProperties( Properties::Array& Properties );
    virtual void SetProperties( Properties::Array Properties );

    virtual pcstr* GetObjectTypes( void );
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    virtual ISystemTask* GetSystemTask( void );

    virtual System::Changes::BitMask GetPotentialSystemChanges( void );
    virtual System::Changes::BitMask GetDesiredSystemChanges( void );

    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

public:
    
    ///// <summary >
    ///// WaterStream camera position used to recycle water "comets" from impact with an 
    ///// object back to the point of origin
    ///// </summary >
    //Base::Vector3 m_CameraPosition;

    //Base::Vector3 m_CameraLookAt;

protected:

    WaterTask*                 m_pWaterTask;    // Main task for this scen
    std::vector<WaterObject*>  m_Objects;   // Scene objects
    int                        m_StreamObjectID;
};

