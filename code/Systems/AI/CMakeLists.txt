project (SystemAI)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/AI/CamBot.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/SystemAI.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Task.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Bots/Animal.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Bots/Bot.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Bots/Chicken.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Bots/Horse.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Bots/Swallow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/Flocking.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/Goal.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/GotoPosition.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/Herding.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/Idle.cpp
        ${CMAKE_SOURCE_DIR}/Systems/AI/Goals/Panic.cpp

    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemAI SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemAI ${SYSTEM_LIBRARIES})
    set_target_properties(SystemAI PROPERTIES PREFIX "")
