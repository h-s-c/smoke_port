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


class OGREGraphicsScene;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Implementation of the ISystemTask interface for OGRE graphics.
///   See Interfaces\System.h for a definition of the class and its functions.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class OGREGraphicsTask : public ISystemTask
{
    friend OGREGraphicsScene;


protected:

    OGREGraphicsTask( OGREGraphicsScene* pScene, Ogre::Root* pRoot );
    virtual ~OGREGraphicsTask( void );

    /// <summary cref="OGREGraphicsTask::GetSystemType">
    ///   Implementation of the <c>ISystemTask::GetSystemType</c> function.
    ///   Gets the system type for this system task.
    /// </summary>
    /// <returns>System::Type - The type of the system.</returns>
    /// <seealso cref="ISystemTask::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="OGREGraphicsTask::Update">
    ///   Implementation of the <c>ISystemTask::Update</c> function.
    ///   Function informing the task to perform its updates.  This does
    ///   all the work needed to update graphics for this frame.
    /// </summary>
    /// <param name="DeltaTime">The time delta from the last call.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) { return false; } 


private:

    OGREGraphicsScene*                  m_pScene;
    Ogre::Root*                         m_pRoot;
};

