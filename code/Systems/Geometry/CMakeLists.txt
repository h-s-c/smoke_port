project (SystemGeometry)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/Geometry/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Geometry/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Geometry/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Geometry/SystemGeometry.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemGeometry SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemGeometry ${SYSTEM_LIBRARIES})
    set_target_properties(SystemGeometry PROPERTIES PREFIX "")

    install (TARGETS SystemGeometry
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemGeometry
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/debug 
        CONFIGURATIONS Debug)  
