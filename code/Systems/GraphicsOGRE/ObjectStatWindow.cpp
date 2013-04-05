//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/Common/System.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectStatWindow.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


extern ManagerInterfaces       g_Managers;

// Define update constants
const f32 OGREGraphicsObjectStatWindow::m_secondsPerUpdate = 1.0f;



//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectStatWindow::OGREGraphicsObjectStatWindow
// 
// Stat window constructor.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectStatWindow::OGREGraphicsObjectStatWindow(ISystemScene* pSystemScene, pcstr pszName) :
    OGREGraphicsObjectWindow( pSystemScene, pszName ),
    m_secondsSinceLastUpdate( 0.0f ),
    m_framesSinceLastUpdate( 0 )
{
    m_Type = OGREGraphicsObject::Type_StatWindow;
}


//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectStatWindow::~OGREGraphicsObjectStatWindow
// 
// Stat window destructor.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectStatWindow::~OGREGraphicsObjectStatWindow()
{
    // DONOTHING
}


//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectStatWindow::Update
// 
// Get updates for this window, and apply them to the window.  Called every time there is an update.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectStatWindow::Update( f32 DeltaTime )
{
    // Don't do anything if this window is not visible.
    if (!this->m_pWindow->isVisible()) {
        return;
    }

    // Only get it if it has been a while.
    m_framesSinceLastUpdate++;
    m_secondsSinceLastUpdate += DeltaTime;

    if (m_secondsSinceLastUpdate > m_secondsPerUpdate) {
        // Get whatever data we need for this window and apply it; do it again
        // to make sure the caption is shown, too.
        WindowData* pWindowData = GetWindowDataUpdate();
        ApplyChanges( pWindowData );
        pWindowData->nFlags = WINDOW_CAPTION;
        ApplyChanges( pWindowData );
    
        // Start counting again toward the next update.
        m_secondsSinceLastUpdate = 0.0f;
        m_framesSinceLastUpdate = 0;
    }
}


//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectStatWindow::GetWindowDataUpdate
// 
// This method gets the latest performance stats, and formats them for the caller.
//
//-----------------------------------------------------------------------------
WindowData* OGREGraphicsObjectStatWindow::GetWindowDataUpdate()
{
    // Get instrumentation updates.
    /*IService::IInstrumentation& inst = g_Managers.pService->Instrumentation();

    // Get the basic stats: average FPS of recent frames, and thread count.
    f32 frameRate = inst.getCurrentFPS();
    i32 activeThreads = inst.getActiveThreadCount();*/

    // Format the start of the stream.
    std::stringstream outputStream;
    outputStream << " " << 0 /*(int)frameRate*/ << " FPS\n";
    outputStream << " " << 0 /*activeThreads*/ << " thread";
    /*if ( activeThreads != 1 ) { 
        outputStream << "s";
    }*/
    
    // One counter per processor, plus one for _Total.
    /*i32 numCounters = inst.getNumCounters();
    f64* CPUPercent = new f64[numCounters];
    if (CPUPercent == NULL) {
        return NULL;
    }
    inst.getCPUCounters( CPUPercent );*/

    // Get all individual CPU counters; don't show the last one ("Total") because it's shown elsewhere.
    /*for (int i = 0; i < numCounters-1; i++) {
        outputStream << "\n";
        // This code "guesses" at the order of the counters, and figures that counters are passed
        // in the order 0-n, Total.  This could be made more automatic, but we'd need 
        // instrumentation code to tell us the actual order.
        //
        // Processors are numbered from 0.  Number them from 1 here to be a bit more user friendly.
        outputStream << " CPU " << i+1 << ": " << (int)CPUPercent[i] << "%";
    }
    delete [] CPUPercent;*/
    outputStream << " CPU " << 0 << ": " << 0 << "%";

    // Arrange the data for display.
    m_windowData.sBody = outputStream.str();
    m_windowData.sCaption = "Performance";
    m_windowData.nFlags = WINDOW_BODY;

    // Set the unused fields, for completeness.
    m_windowData.sName = "";
    m_windowData.bShow = true;
    m_windowData.nDataPoint = 0.0f;

    return &m_windowData;
}
