project (SystemInput)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/Input/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Input/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Input/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Input/SystemInput.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Input/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##ois
    find_package(OIS REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${OIS_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${OIS_LIBRARIES}) 
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemInput SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemInput ${SYSTEM_LIBRARIES})
    set_target_properties(SystemInput PROPERTIES PREFIX "")
