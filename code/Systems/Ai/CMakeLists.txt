# public domain

project (SystemAi)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/ObjectCamBot.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/SystemAi.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Task.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Bots/Animal.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Bots/Bot.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Bots/Chicken.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Bots/Horse.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Bots/Swallow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/Flocking.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/Goal.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/GotoPosition.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/Herding.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/Idle.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Ai/Goals/Panic.cpp

    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemAi SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemAi ${SYSTEM_LIBRARIES})
    set_target_properties(SystemAi PROPERTIES PREFIX "")
    
    install (TARGETS SystemAi
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemAi
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
