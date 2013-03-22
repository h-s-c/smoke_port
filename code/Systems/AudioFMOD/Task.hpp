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

class FMODScene;

/*******************************************************************************
* CLASS: FMODTask
*
* DESCRIPTION:
* Implementation of the ISystemTask interface for FMOD audio.
* See Interfaces\System.h for a definition of the class and its functions.
*******************************************************************************/

class FMODTask : public ISystemTask
{
    friend class FMODScene;

protected:

    FMODTask( FMODScene* pScene );
    virtual ~FMODTask( void );

    /// <summary cref="ISystemScene::GetSystemType">
    ///   Implementation of the ISystemScene GetSystemType function.
    /// </summary>
    virtual System::Type GetSystemType( void );

    virtual void Update( f32 DeltaTime );

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) { return false; } 

private:

    FMODScene*  m_pScene;   // Pointer to system scene
    FMODSystem* m_pSystem;  // Pointer to system
    Bool        m_Pause;    // Game is paused?
    FMOD_RESULT m_Result;   // FMOD Error
};

