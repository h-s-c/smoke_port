project (SystemExplosion)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/ObjectExplosion.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/ObjectMeteorImpact.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/SystemExplosion.cpp
        ${CMAKE_SOURCE_DIR}/Systems/Explosion/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemExplosion SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemExplosion ${SYSTEM_LIBRARIES})
    set_target_properties(SystemExplosion PROPERTIES PREFIX "")

    install (TARGETS SystemExplosion
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemExplosion
        DESTINATION ${CMAKE_SOURCE_DIR}/../bin/debug 
        CONFIGURATIONS Debug)  
