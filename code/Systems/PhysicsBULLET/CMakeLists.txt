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

project (SystemPhysicsBULLET)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ObjectCharacter.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ObjectPhysics.cpp        
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/ServiceCollsion.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/SystemPhysicsBULLET.cpp
        ${CMAKE_SOURCE_DIR}/Systems/PhysicsBULLET/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    ##bullet
    find_package(Bullet REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${BULLET_LIBRARIES}) 
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemPhysicsBULLET SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemPhysicsBULLET ${SYSTEM_LIBRARIES})
    set_target_properties(SystemPhysicsBULLET PROPERTIES PREFIX "")

    install (TARGETS SystemPhysicsBULLET
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemPhysicsBULLET
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
