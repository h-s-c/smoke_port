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


/// <summary>
/// Workload window.  Show relative workloads in different systems.
/// </summary>
class OGREGraphicsObjectWorkloadWindow : public OGREGraphicsObjectWindow
{
    friend OGREGraphicsScene;

protected:

	/// <summary cref="OGREGraphicsObjectWorkloadWindow::OGREGraphicsObjectWorkloadWindow">
	///		Basic constructor; the only real one.
	/// </summary>
	/// <param name="pSystemScene">ISystemScene* - Pointer to the scene containing this window.</param>
	/// <param name="pszName">pcstr - Name of this window.</param>
    OGREGraphicsObjectWorkloadWindow( ISystemScene* pSystemScene, pcstr pszName );

	/// <summary cref="OGREGraphicsObjectWorkloadWindow::~OGREGraphicsObjectWorkloadWindow">
	///		Basic destructor; the only one.
	/// </summary>
    ~OGREGraphicsObjectWorkloadWindow( void );

	/// <summary cref="OGREGraphicsObjectWorkloadWindow::Update">
	///		Called by the framework every frame, so this window can update itself.
	/// </summary>
	/// <param name="DeltaTime">f32 - Elapsed wall-clock time since the last call to this function.</param>
    virtual void Update( f32 DeltaTime );

private:
    static const f32					m_secondsPerUpdate;
    f32									m_secondsSinceLastUpdate;
    i32									m_framesSinceLastUpdate;
	WindowData							m_windowData;

	/// <summary cref="OGREGraphicsObjectWorkloadWindow::GetWindowDataUpdate">
	///		Implementation function, used by the Update function.
	/// </summary>
	/// <returns>WindowData* - Result structure containing all data to update in the UI.</returns>
	virtual WindowData*					GetWindowDataUpdate();
};
