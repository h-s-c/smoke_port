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

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) { return false; } 


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

    btDiscreteDynamicsWorld*                m_pWorld;

    std::list<BulletPhysicsObject*>         m_ActiveObjects;

    f32                                     m_DeltaTime;
};

