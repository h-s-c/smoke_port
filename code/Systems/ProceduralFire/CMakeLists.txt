## The MIT License (MIT)
## Copyright (c) 2013 Kevin Schmidt
##  
## Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
## associated documentation files (the "Software"), to deal in the Software without restriction, 
## including without limitation the rights to use, copy, modify, merge, publish, distribute, 
## sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
## furnished to do so, subject to the following conditions:
##  
## The above copyright notice and this permission notice shall be included in all copies or 
## substantial portions of the Software.
##  
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
## NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
## NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
## DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
