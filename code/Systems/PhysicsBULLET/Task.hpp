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


class BulletPhysicsScene;
class BulletPhysicsObject;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>BulletPhysicsTask</c> Implementation of the ISystemTask interface for
///   BulletPhysics.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class BulletPhysicsTask : public ISystemTask,
                         public hkpEntityListener, public hkpEntityActivationListener
{
    friend BulletPhysicsScene;


protected:

    BulletPhysicsTask( BulletPhysicsScene* pScene, hkpWorld* pWorld );
    ~BulletPhysicsTask( void );

    /// <summary cref="BulletPhysicsTask::GetSystemType">
    ///   Implementation of the <c>ISystemTask::GetSystemType</c> function.
    ///   Gets the system type for this system task.
    /// </summary>
    /// <returns>System::Type - The type of the system.</returns>
    /// <seealso cref="ISystemTask::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="BulletPhysicsTask::SetObjectActivation">
    ///   Add or removes the given object from actively tracked objects.
    /// </summary>
    /// <param name="pObject">The Object that we want to activate/deactivate.</param>
    /// <param name="bActivated">True marks the Object as active.</param>
    void SetObjectActivation( BulletPhysicsObject* pObject, Bool bActivated=True );

    /// <summary cref="BulletPhysicsTask::Update">
    ///   Implementation of the <c>ISystemTask::Update</c> function.
    ///   Function informing the task to perform its updates.  This does
    ///   all the work needed to update AI for this frame.
    /// </summary>
    /// <param name="DeltaTime">The time delta from the last call.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /// <summary cref="BulletPhysicsTask::StepUpdate">
    ///   Advance the world owned by this thread.
    /// </summary>
    /// <param name="ThreadId">The unique ID for this thread.</param>
    void StepUpdate( u32 ThreadId );

    /// <summary cref="BulletPhysicsTask::UpdateCompletion">
    ///   Callback function called when all threads have finished.
    /// </summary>
    void UpdateCompletion( void );

    /// <summary cref="BulletPhysicsTask::StepUpdateS">
    ///   Advance the world owned by this thread (static version).
    /// </summary>
    /// <param name="pTask">Pointer to Task that generated this call.</param>
    /// <param name="uStart">Start time (currently unused)</param>
    /// <param name="uEnd">End time (currently unused)</param>
    static void StepUpdateS( BulletPhysicsTask* pTask, u32 uStart, u32 uEnd );

    /// <summary cref="BulletPhysicsTask::IsPrimaryThreadOnly">
    ///   Implementation of the <c>ISystemTask::IsPrimaryThreadOnly</c> function.
    ///   This function returns False for BulletPhysicsTask.  BulletPhysicsTask does 
    ///   not need to run only on the primary thread.
    /// </summary>
    /// <returns>Bool - Returns if this system should only run on the primary thread.</returns>
    /// <seealso cref="ISystemTask::IsPrimaryThreadOnly"/>
    virtual Bool IsPrimaryThreadOnly( void ) { return False; } 


private:

    /// <summary cref="BulletPhysicsTask::entityAddedCallback">
    ///   Callback for when an entity is added by Bullet.
    ///   (See Bullet documentation for more details)
    /// </summary>
    /// <param name="pEntity">Bullet entity that was added.</param>
    virtual void entityAddedCallback( hkpEntity* pEntity );

    /// <summary cref="BulletPhysicsTask::entityRemovedCallback">
    ///   Callback for when an entity is removed by Bullet.
    ///   (See Bullet documentation for more details)
    /// </summary>
    /// <param name="pEntity">Bullet entity that was removed.</param>
    virtual void entityRemovedCallback( hkpEntity* pEntity );

    /// <summary cref="BulletPhysicsTask::entityDeactivatedCallback">
    ///   Callback for when an entity is deactived by Bullet.
    ///   (See Bullet documentation for more details)
    /// </summary>
    /// <param name="pEntity">Bullet entity that was deactivated.</param>
    virtual void entityDeactivatedCallback( hkpEntity* pEntity );

    /// <summary cref="BulletPhysicsTask::entityActivatedCallback">
    ///   Callback for when an entity is actived by Bullet.
    ///   (See Bullet documentation for more details)
    /// </summary>
    /// <param name="pEntity">Bullet entity that was activated.</param>
    virtual void entityActivatedCallback( hkpEntity* pEntity );


private:
    BulletPhysicsScene*                     m_pScene;

    btDiscreteDynamicsWorld*                       m_pWorld;

    std::list<BulletPhysicsObject*>         m_ActiveObjects;

    //u32                                     m_cJobs;

    //u32                                     m_PrimaryThreadId;
    //hkpThreadToken                          m_PrimaryThreadToken;

    f32                                     m_DeltaTime;
};

