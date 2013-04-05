//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <OgreBorderPanelOverlayElement.h>
#include <OgreHardwarePixelBuffer.h>
//system
#include "Systems/Common/System.hpp"
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/ObjectWorkloadWindow.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


extern ManagerInterfaces       g_Managers;

// Define update constants
const f32 OGREGraphicsObjectWorkloadWindow::m_secondsPerUpdate = 1.0f;

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWorkloadWindow::OGREGraphicsObjectWorkloadWindow
// 
// Constructor for workload window.  Used to show relative workloads between systems.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectWorkloadWindow::OGREGraphicsObjectWorkloadWindow(ISystemScene* pSystemScene, pcstr pszName) :
    OGREGraphicsObjectWindow( pSystemScene, pszName ),
    m_secondsSinceLastUpdate( 0.0f ),
    m_framesSinceLastUpdate( 0 )
{
    m_Type = OGREGraphicsObject::Type_WorkloadWindow;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWorkloadWindow::~OGREGraphicsObjectWorkloadWindow
// 
// Destructor for workload window.
//
//-----------------------------------------------------------------------------
OGREGraphicsObjectWorkloadWindow::~OGREGraphicsObjectWorkloadWindow(void)
{
    // DONOTHING
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWorkloadWindow::Update
// 
// Get updates for this window, and apply them to the window.  Called every time there is an update.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWorkloadWindow::Update( f32 DeltaTime )
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
WindowData* OGREGraphicsObjectWorkloadWindow::GetWindowDataUpdate( )
{
    // Get instrumentation updates.
    /*IService::IInstrumentation& inst = g_Managers.pService->Instrumentation();

    // Get access to Audio system for audio data
    ISystem* pAudioSystem = (ISystem*) g_Managers.pService->SystemAccess().GetSystem( System::Types::Audio );

    // Format the start of the stream.
    std::stringstream outputStream;
    i32 cpuCount = inst.getCPUCount();
    i32 jobCount = inst.getJobCount();

    // Get the workload job ratios.
    f32* jobRatios = new f32[jobCount];
    if ( jobRatios == NULL ) {
        return ( NULL );
    }
    inst.getJobRatios( jobRatios );

    // Hook up the display names.
    bool indexUsed;
    for (int i = 0; i < jobCount; i++) {
        indexUsed = false;
        switch (i) {
            case System::TypeIndices::Geometry:
                outputStream << " " << System::DisplayNames::Geometry << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Graphics:
                outputStream << "\n " << System::DisplayNames::Graphics << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::PhysicsCollision:
                outputStream << "\n " << System::DisplayNames::PhysicsCollision << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Audio:
                outputStream << "\n " << System::DisplayNames::Audio << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Input:
                outputStream << "\n " << System::DisplayNames::Input << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::AI:
                outputStream << "\n " << System::DisplayNames::AI << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Animation:
                outputStream << "\n " << System::DisplayNames::Animation << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Scripting:
                outputStream << "\n " << System::DisplayNames::Scripting << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Explosion:
                outputStream << "\n " << System::DisplayNames::Explosion << ": ";
                indexUsed = true;
                break;

            case System::TypeIndices::Water:
                outputStream << "\n " << System::DisplayNames::Water << ": ";
                indexUsed = true;
                break;

            default:
                // Show the custom types, if we've found them.
                if ( i == (int)System::Types::GetIndex ( System::Types::MakeCustom ( 0 ) ) ) {
                    outputStream << "\n " << System::DisplayNames::Tree << ": ";
                    indexUsed = true;
                } else if ( i == (int)System::Types::GetIndex ( System::Types::MakeCustom ( 2 ) ) ) {
                    outputStream << "\n " << System::DisplayNames::Fire << ": ";
                    indexUsed = true;
                } else {
                    // There are lots of unused system type slots; don't display
                    // them unless we add a new system type.

                    // Consider adding some special handing of System::Types::Null, in case
                    // somebody accidentally files some job workloads there.

                    // DONOTHING
                }

                break;
        }

        // The job array has a few slots we don't use.  If we aren't keeping stats for this index,
        // don't try to display it.
        if ( indexUsed ) {
            // Job ratios show the amount of one available core that a job is using, so they're a kind
            // of utilization number.  On a 4 core machine, they can vary between 0 and 4.
            // Scale it here to a percent of the overall workload.
            f32 rawPercent;

            // Audio is a special case; it has its own internal thread not scheduled or tracked
            // through the job system, so we need to get stats differently (directly from the
            // audio system).
            if (i == System::Types::Audio) {
                rawPercent = pAudioSystem->GetCPUUsage();
            } else {
                rawPercent = ((jobRatios[i] * 100.0f)/(f32)cpuCount);
            }

            // Round to nearest integer, then make sure each subsystem has something to report.
            i32 percent = (i32)( rawPercent + 0.5f );
            if (percent == 0) {
                // A zero makes it look like we're claiming a subsystem is totally idle, so just claim 1%.
                outputStream << "1%";  // In this font, "<" looks like an arrow and "~" looks like a "-".  Neither looks right.
            } else {
                outputStream << percent << "%";
            }
        }
    }

    // Clean up.
    delete [] jobRatios;

    // Arrange the fields for display; most fields are filled already.
    m_windowData.sBody = outputStream.str();*/
    m_windowData.sBody = "NULL";
    m_windowData.sCaption = "Workload percentages";
    m_windowData.nFlags = WINDOW_BODY;

    // Set the unused fields, for completeness.
    m_windowData.sName = "";
    m_windowData.bShow = true;
    m_windowData.nDataPoint = 0.0f;

    return &m_windowData;
}
