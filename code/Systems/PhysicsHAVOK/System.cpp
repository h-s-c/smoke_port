// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Extern
#include <Common\Base\hkBase.h>
#include <Common\Base\Memory\Memory\Pool\hkPoolMemory.h>
//#include <Common\Base\Memory\Memory\Debug\hkStlDebugMemory.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
// System
#include "Systems/PhysicsHAVOK/ServiceCollision.hpp"
#include "Systems/PhysicsHAVOK/System.hpp"
#include "Systems/PhysicsHAVOK/Scene.hpp"

extern ManagerInterfaces    g_Managers;


///////////////////////////////////////////////////////////////////////////////
// HavokPhysicsSystem - Constructor
HavokPhysicsSystem::HavokPhysicsSystem(
    void
    )
    : ISystem()
    , m_pThreadMemoryStack( NULL )
    , m_tlsHavocMemory(TLS_OUT_OF_INDEXES)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~HavokPhysicsSystem - Destructor
HavokPhysicsSystem::~HavokPhysicsSystem(
    void
    )
{
}


///////////////////////////////////////////////////////////////////////////////
// GetName - Returns the name of this System
pcstr
HavokPhysicsSystem::GetName(
    void
    )
{
    return System::Names::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// GetSystemType - Returns System type for this System
System::Type
HavokPhysicsSystem::GetSystemType(
    void
    )
{
    return System::Types::PhysicsCollision;
}


///////////////////////////////////////////////////////////////////////////////
// Initialize - Initializes this System with the given properties
Error
HavokPhysicsSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// GetProperties - Properties for this System are returned in Properties
void
HavokPhysicsSystem::GetProperties(
    Properties::Array& Properties
    )
{
    UNREFERENCED_PARAM( Properties );
}


///////////////////////////////////////////////////////////////////////////////
// SetProperties - Set properties for this System
void
HavokPhysicsSystem::SetProperties(
    Properties::Array Properties
    )
{
    UNREFERENCED_PARAM( Properties );

    ASSERT( m_bInitialized );
}

u32 HavokPhysicsSystem::s_idMainThread = 0;

///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene*
HavokPhysicsSystem::CreateScene(
    void
    )
{
    ASSERT( m_pThreadMemoryStack == NULL );

    //
    // Initialize Havok.
    //
#ifdef _DEBUG_WITH_MEM
    hkMemory* pMemory = new hkStlDebugMemory();
#else
    hkMemory* pMemory = new hkPoolMemory();
#endif
    hkThreadMemory* pThreadMemory = new hkThreadMemory( pMemory );
    hkBaseSystem::init( pMemory, pThreadMemory, ErrorReport, this );

    //
    // Allocate memory and set it as the stack area for the Havok threads.
    //
    m_pThreadMemoryStack = _aligned_malloc( m_cThreadMemoryStack, 16 );
    ASSERT( m_pThreadMemoryStack != NULL );

    pThreadMemory->setStackArea( m_pThreadMemoryStack, m_cThreadMemoryStack );

    //
    // Disable some unnecessary warnings.
    //
    hkError::getInstance().setEnabled( 0x2a2cde91, false );
    hkError::getInstance().setEnabled( 0x6e8d163b, false );
    hkError::getInstance().setEnabled( 0xad345a23, false );
    hkError::getInstance().setEnabled( 0x11fce585, false );
    hkError::getInstance().setEnabled( 0x34df5494, false );
#ifdef __HAVOK_VDB__
    hkError::getInstance().setEnabled( 0x1293ADE8, false );
    hkError::getInstance().setEnabled( 0x1293ADEF, false );
#endif

    //
    // Remove the references to the memory as the base system owns them now.
    //
    pMemory->removeReference();
    pThreadMemory->removeReference();

    //
    // Allocate memory for each thread via a callback.
    //
    if( g_Managers.pTask != NULL )
    {
        s_idMainThread = ::GetCurrentThreadId();
        m_tlsHavocMemory = ::TlsAlloc();
        ASSERT (m_tlsHavocMemory != TLS_OUT_OF_INDEXES && "HavokPhysicsSystem: Not enough space in TLS");

        g_Managers.pTask->PerThreadCallback(
            reinterpret_cast<ITaskManager::JobFunction>(AllocateThreadResources), this
            );
    }
    else
    {
        AllocateThreadResources( this );
    }

    return new HavokPhysicsScene( this );
}


///////////////////////////////////////////////////////////////////////////////
// DestroyScene - Destroys the given Scene, free all associated resources
Error
HavokPhysicsSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    HavokPhysicsScene* pScene = reinterpret_cast<HavokPhysicsScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    //
    // Free memory for each thread via a callback.
    //
    if( g_Managers.pTask != NULL )
    {
        g_Managers.pTask->PerThreadCallback(
            reinterpret_cast<ITaskManager::JobFunction>(FreeThreadResources), this
            );
    }
    else
    {
        //FreeThreadResources( this );
    }

    //
    // Free up Havok.
    //
    //hkBaseSystem::clearThreadResources();
    hkBaseSystem::quit();

    if ( m_tlsHavocMemory != TLS_OUT_OF_INDEXES ) {
        ::TlsFree(m_tlsHavocMemory);
        m_tlsHavocMemory = TLS_OUT_OF_INDEXES;
    }

    //
    // Free the stack memory.
    //
    if ( m_pThreadMemoryStack != NULL )
    {
        _aligned_free( m_pThreadMemoryStack );
        m_pThreadMemoryStack = NULL;
    }

    return Errors::Success;
}


///////////////////////////////////////////////////////////////////////////////
// AllocateThreadResources - Allocate resources need to support multiple threads
void
HavokPhysicsSystem::AllocateThreadResources(
    HavokPhysicsSystem* pSystem
    )
{
    //
    // Create thread memory for the thread.
    //
    hkThreadMemory* pThreadMemory = new hkThreadMemory( &hkMemory::getInstance() );
    hkBaseSystem::initThread( pThreadMemory );

    //
    // Allocate memory for the thread's stack.
    //
    void* pStack = _aligned_malloc( m_cThreadMemoryStack, 16 );
    pThreadMemory->setStackArea( pStack, m_cThreadMemoryStack );

    ::TlsSetValue(pSystem->m_tlsHavocMemory, pStack);
}


///////////////////////////////////////////////////////////////////////////////
// FreeThreadResources - Free resources allocated to support multiple threads
void
HavokPhysicsSystem::FreeThreadResources(
    HavokPhysicsSystem* pSystem
    )
{
    //
    // If the main thread serves as a worker of the scheduler's thread pool, 
    // do not deinitialize havok's data here, 'cause doing so crashes hkBaseSystem::quit()
    //
    if ( ::GetCurrentThreadId() == s_idMainThread )
        return;

    //
    // Free up Havok resources and free the stack space.
    //
    hkBaseSystem::clearThreadResources();

    _aligned_free( ::TlsGetValue(pSystem->m_tlsHavocMemory) );
}


///////////////////////////////////////////////////////////////////////////////
// ErrorReport - Callback for Havok to report an error
void
HavokPhysicsSystem::ErrorReport(
    pcstr pString,
    void* pErrorOutputObject
    )
{
    HavokPhysicsSystem* pSystem = reinterpret_cast<HavokPhysicsSystem*>(pErrorOutputObject);

    //Debug::Print( "[%s Error]: %s", pSystem->GetName(), pString );

    ASSERTMSG( False, pString );
}
