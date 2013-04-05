// Base
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
// Interface
#include "Interfaces/Interface.hpp"
// Extern
#include <Common\Base\hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Container/String/hkStringBuf.h>
#include <Physics\Dynamics\World\hkpWorld.h>
#include <Physics\Utilities\Destruction\BreakOffParts\hkpBreakOffPartsUtil.h>
// Platform initialization
#include <Common/Base/System/Init/PlatformInit.cxx>
// System
#include "Systems/Common/System.hpp"
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

///////////////////////////////////////////////////////////////////////////////
// CreateScene - Creates and returns a new Scene
ISystemScene*
HavokPhysicsSystem::CreateScene(
    void
    )
{
    //
    // Initialize Havok.
    //
	PlatformInit();
	hkMemorySystem::FrameInfo finfo(500 * 1024);	// Allocate 500KB of Physics solver buffer
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, finfo);
	hkBaseSystem::init( memoryRouter, ErrorReport );

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
    // Free up Havok.
    //
    hkBaseSystem::quit();

    return Errors::Success;
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
