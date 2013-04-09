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


class InputScene;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>InputTask</c> Implementation of the ISystemTask interface for Input.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class InputTask : public ISystemTask
{
    friend class InputScene;


protected:

    InputTask( InputScene* pScene );
    virtual ~InputTask( void );

    /// <summary cref="InputTask::GetSystemType">
    ///   Implementation of the <c>ISystemTask::GetSystemType</c> function.
    ///   Gets the system type for this system task.
    /// </summary>
    /// <returns>System::Type - The type of the system.</returns>
    /// <seealso cref="ISystemTask::GetSystemType"/>
    virtual System::Type GetSystemType( void );

    /// <summary cref="InputTask::Update">
    ///   Implementation of the <c>ISystemTask::Update</c> function.
    ///   Function informing the task to perform its updates.  This does
    ///   all the work needed to update Input objects for this frame.
    /// </summary>
    /// <param name="DeltaTime">The time delta from the last call.</param>
    /// <seealso cref="ISystemTask::Update"/>
    virtual void Update( f32 DeltaTime );

    /* tells the taskmanager to always run tasks from this 
     * system on the same thread if they are not thread-safe*/
    virtual bool IsThreadSafe( void ) { return false; } 


private:

    InputScene*                     m_pScene;
};

