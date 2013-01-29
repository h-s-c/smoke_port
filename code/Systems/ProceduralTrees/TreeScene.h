//Writen by Hugh Smith summer and fall 2007
// Part of Smoke Framework implementation for procedural Tree system.

// Copyright � 2008-2009 Intel Corporation
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


class TreeSystem;
class TreeTask;
class TreeObject;

typedef std::list<TreeObject *> TreeObjectList;

/*******************************************************************************
* CLASS: TreeScene
*
* DESCRIPTION:
* Implementation of the ISystemScene interface.
* See Interfaces\System.h for a definition of the class and its functions.
*******************************************************************************/

class TreeScene : public ISystemScene
{
    friend TreeSystem;
    friend TreeTask;


protected:

    TreeScene( ISystem* pSystem );
    ~TreeScene( void );

    /// <summary cref="ISystemScene::GetSystemType">
    ///   Implementation of the ISystemScene GetSystemType function.
    /// </summary>
    virtual System::Type GetSystemType( void );

    virtual Error Initialize( std::vector<Properties::Property> Properties );

    virtual void GetProperties( std::vector<Properties::Property>& Properties );
    virtual void SetProperties( std::vector<Properties::Property> Properties );

    virtual pcstr* GetObjectTypes( void );
    virtual ISystemObject* CreateObject( pcstr pszName, pcstr pszType );
    virtual Error DestroyObject( ISystemObject* pSystemObject );

    virtual ISystemTask* GetSystemTask( void );

    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

	virtual const void* GetSystemChangeData( System::Change SystemChange );
public:
    virtual TreeObjectList GetForest();


protected:

    //enum PropertyTypes
    //{
    //    Property_Gravity,
    //    Property_Count
    //};

    //static pcstr                        sm_kapszPropertyNames[];
    //static const Properties::Property   sm_kaDefaultProperties[];

    TreeTask*                     m_pTask;
    TreeObjectList       m_Forest;
};
