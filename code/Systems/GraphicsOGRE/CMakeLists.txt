project (SystemGraphicsOGRE)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicBuffer.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicLines.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicRenderable.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/MovableText.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/NormalizedLines.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectCamera.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectChart.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectCPUChart.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectLight.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectLightFire.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectMesh.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectMeshAnimated.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectPagedGeometryLayer.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectParticles.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectParticleSystem.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectStatWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectWorkloadWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/SystemGraphicsOGRE.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##ogre
    find_package(OGRE REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_INCLUDE_DIR})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_RenderSystem_GL_INCLUDE_DIRS})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_Plugin_ParticleFX_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_LIBRARIES}) 
    list(APPEND SYSTEM_LIBRARIES ${OGRE_RenderSystem_GL_LIBRARIES})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_Plugin_ParticleFX_LIBRARIES})
    find_package(PagedGeometry REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${PG_INCLUDE_DIR})
    list(APPEND SYSTEM_LIBRARIES ${PG_LIBRARIES}) 
	
	##ogre sdk workaround
	if (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
		set(BOOST_ROOT 			"C:/OgreSDK/boost")
		set(Boost_USE_STATIC_LIBS ON)
		find_package(Boost COMPONENTS thread)
		list(APPEND SYSTEM_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
		list(APPEND SYSTEM_LIBRARIES ${Boost_LIBRARIES}) 
	endif ()
	
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)

    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemGraphicsOGRE SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemGraphicsOGRE ${SYSTEM_LIBRARIES})
    set_target_properties(SystemGraphicsOGRE PROPERTIES PREFIX "")
