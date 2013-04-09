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
        
if(UNIX)
    install (FILES ${FMOD_LIBRARY} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo)
    install (FILES ${FMOD_LIBRARY} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug)
endif()
