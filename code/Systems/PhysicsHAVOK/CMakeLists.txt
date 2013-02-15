project (SystemPhysicsHAVOK)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/Extras/PhantomShape.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/ObjectCharacter.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/ObjectPhysics.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/ServiceCollision.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/SystemPhysicsHAVOK.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsHAVOK/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##havok
    find_package(HAVOK REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${HAVOK_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${HAVOK_LIBRARIES}) 
	
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemPhysicsHAVOK SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemPhysicsHAVOK ${SYSTEM_LIBRARIES})
    set_target_properties(SystemPhysicsHAVOK PROPERTIES PREFIX "")
