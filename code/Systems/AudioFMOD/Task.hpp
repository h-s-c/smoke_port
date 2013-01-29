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
    virtual Bool IsPrimaryThreadOnly( void ) { return True; } 

private:

    FMODScene*  m_pScene;   // Pointer to system scene
    FMODSystem* m_pSystem;  // Pointer to system
    Bool        m_Pause;    // Game is paused?
    FMOD_RESULT m_Result;   // FMOD Error
};

