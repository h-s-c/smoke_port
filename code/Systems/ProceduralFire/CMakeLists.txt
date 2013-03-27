# public domain

project (SystemProceduralFire)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/SystemProceduralFire.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/Task.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/ColdParticle.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/Fire.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/FireBall.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/FirePatch.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/HeatParticle.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralFire/ParticleEmitter/ParticleSystem.cpp

    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemProceduralFire SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemProceduralFire ${SYSTEM_LIBRARIES})
    set_target_properties(SystemProceduralFire PROPERTIES PREFIX "")
    
    install (TARGETS SystemProceduralFire
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemProceduralFire
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
