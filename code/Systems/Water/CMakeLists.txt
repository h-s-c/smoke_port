# public domain

project (SystemWater)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/Water/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Water/ObjectWaterStream.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Water/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Water/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Water/SystemWater.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Water/Task.cpp

    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemWater SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemWater ${SYSTEM_LIBRARIES})
    set_target_properties(SystemWater PROPERTIES PREFIX "")
    
    install (TARGETS SystemWater
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemWater
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
