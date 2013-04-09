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
