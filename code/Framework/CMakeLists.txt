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

include(ExternalProject)

project (Framework)
    set( FRAMEWORK_SOURCE
        ${CMAKE_SOURCE_DIR}/Framework/ChangeControlManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/EnvironmentManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/Framework.cpp
        ${CMAKE_SOURCE_DIR}/Framework/PlatformManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/Scheduler.cpp
        ${CMAKE_SOURCE_DIR}/Framework/ServiceManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/SystemManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/TaskManager.cpp
        ${CMAKE_SOURCE_DIR}/Framework/Universal.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})
   
    if (UNIX) 
        ##tinyxml
        find_package( TinyXML REQUIRED)
        list(APPEND FRAMEWORK_LIBRARIES ${TINYXML_LIBRARIES})
        list(APPEND FRAMEWORK_INCLUDE_DIRS ${TINYXML_INCLUDE_DIRS})
        ##boost
        find_package( Boost COMPONENTS filesystem system REQUIRED)
        list(APPEND FRAMEWORK_LIBRARIES ${Boost_LIBRARIES})
        list(APPEND FRAMEWORK_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
        list(APPEND FRAMEWORK_LIBRARY_DIRS ${Boost_LIBRARY_DIR})
        ##pthreads
        find_package(Threads)
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
        ##dlfcn
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_DL_LIBS})
    else (MSVC)
        ##tinyxml
        list(APPEND FRAMEWORK_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinystr.cpp)
        list(APPEND FRAMEWORK_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxml.cpp)
        list(APPEND FRAMEWORK_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlerror.cpp)
        list(APPEND FRAMEWORK_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlparser.cpp)
        list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/External/tinyxml)        
        ##dlfcn-win32
        list(APPEND FRAMEWORK_SOURCE ${CMAKE_SOURCE_DIR}/External/dlfcn-win32/dlfcn.c)
        list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/External/dlfcn-win32)
    endif () 
   
    ##base
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
