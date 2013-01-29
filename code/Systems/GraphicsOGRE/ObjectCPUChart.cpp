//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectChart.hpp"
#include "Systems/GraphicsOGRE/ObjectCPUChart.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


extern ManagerInterfaces       g_Managers;

// Define update constants
const f32 OGREGraphicsObjectCPUChart::m_secondsPerUpdate = 1.0f;


//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectCPUChart::OGREGraphicsObjectCPUChart
//
// CPU utilization charting object.  Constructor.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectCPUChart::OGREGraphicsObjectCPUChart(ISystemScene* pSystemScene, pcstr pszName) :
    OGREGraphicsObjectChart( pSystemScene, pszName ),
    m_secondsSinceLastUpdate( 0.0f ),
    m_framesSinceLastUpdate( 0 )
{
    m_Type = OGREGraphicsObject::Type_CPUChart;
    m_yAxisMax = 100.0f;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectCPUChart::~OGREGraphicsObjectCPUChart
//
// Destructor.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectCPUChart::~OGREGraphicsObjectCPUChart(void)
{
    // DONOTHING
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectCPUChart::GetWindowDataUpdate
//
// Get a WindowData object for the chart to show.  This gets its data from the Instrumentation
// object.
//
//-----------------------------------------------------------------------------
WindowData* OGREGraphicsObjectCPUChart::GetWindowDataUpdate()
{
    // Get instrumentation updates.
    /*IService::IInstrumentation& inst = g_Managers.pService->Instrumentation();

    // One counter per processor, plus one for _Total.
    i32 numCounters = inst.getNumCounters();
    f64* CPUPercent = new f64[numCounters];
    if (CPUPercent == NULL) {
        return NULL;
    }
    inst.getCPUCounters( CPUPercent );*/

    // Arrange the data for display.  We have new data and a new caption.
    m_windowData.nDataPoint = 0.0f;//(f32)CPUPercent[numCounters-1];

    std::stringstream captionStream;
    captionStream << "Total CPU: " << 0  /*(int)CPUPercent[numCounters-1]*/ << "%";
    m_windowData.sCaption = captionStream.str();

    m_windowData.nFlags = WINDOW_DATA;

    // Set the unused fields, for completeness.
    m_windowData.sName = "";
    m_windowData.bShow = true;
    m_windowData.sBody = "";

    // Free up locally-allocated stuff.
    //delete [] CPUPercent;

    return &m_windowData;
}


//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectCPUChart::AddDataPoint
//
// Adds a new data point to be graphed.  Overrides the dynamic min/max function
// of the superclass, to limit max to 100.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectCPUChart::AddDataPoint(f32 data)
{
    int i;

    if (!m_pManualForeground) {
        return;
    }

    //-------------------------------------------------------------------------
    // Get rid of our oldest piece of data and add in the newest.
    //
    for (i = 0; i < m_numPoints - 1; i++) {
        m_data[i] = m_data[i + 1];
    }
    m_data[i] = data;

    UpdateChart();

    return;
}



//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectCPUChart::Update
//
// Update gets called when the change manager has a new time slice for us.  DeltaTime tells
// how long it has been since we were last called.  Pull new data when it's needed here.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectCPUChart::Update( f32 DeltaTime )
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
