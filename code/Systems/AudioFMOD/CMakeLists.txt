# public domain

project (SystemAudioFMOD)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/ObjectAI.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/ObjectCollision.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/SystemAudioFMOD.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AudioFMOD/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})

    ##fmod
    find_package(FMOD REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${FMOD_INCLUDE_DIR})
    list(APPEND SYSTEM_LIBRARIES ${FMOD_LIBRARY})   
 
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemAudioFMOD SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemAudioFMOD ${SYSTEM_LIBRARIES})
    set_target_properties(SystemAudioFMOD PROPERTIES PREFIX "")
    set_target_properties(SystemAudioFMOD PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    install (TARGETS SystemAudioFMOD 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo)
    install (TARGETS SystemAudioFMOD 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug)
        
    install (FILES ${FMOD_LIBRARY} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo)
    install (FILES ${FMOD_LIBRARY} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug)
