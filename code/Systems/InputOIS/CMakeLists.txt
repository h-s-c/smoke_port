project (SystemInputOIS)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/InputOIS/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/InputOIS/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/InputOIS/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/InputOIS/SystemInputOIS.cpp
        ${CMAKE_SOURCE_DIR}/Systems/InputOIS/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##ois
    find_package(OIS REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${OIS_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${OIS_LIBRARIES}) 
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemInputOIS SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemInputOIS ${SYSTEM_LIBRARIES})
    set_target_properties(SystemInputOIS PROPERTIES PREFIX "")

    install (TARGETS SystemInputOIS
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemInputOIS
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/debug 
        CONFIGURATIONS Debug)  

if(WIN32)    
    install(FILES ${OGRE_PLUGIN_DIR_REL}/OIS.dll
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/release
        CONFIGURATIONS Release RelWithDebInfo
    )
    install(FILES ${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/debug
        CONFIGURATIONS Debug
    )
endif()
