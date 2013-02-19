project (SystemPhysicsBULLET)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ObjectCharacter.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ObjectPhysics.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ServiceCollsion.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/SystemPhysicsBULLET.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##bullet
    find_package(Bullet REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${BULLET_LIBRARIES}) 
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemPhysicsBULLET SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemPhysicsBULLET ${SYSTEM_LIBRARIES})
    set_target_properties(SystemPhysicsBULLET PROPERTIES PREFIX "")

    install (TARGETS SystemPhysicsBULLET
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemPhysicsBULLET
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/debug 
        CONFIGURATIONS Debug)  
