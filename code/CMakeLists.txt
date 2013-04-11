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

cmake_minimum_required (VERSION 2.8.10)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

project (Smoke)
    # Compiler-specific C++11 activation/checks.
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7))
            message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
        endif ()
        set(CMAKE_CXX_FLAGS                "-Wall -Wextra -Wno-unused -std=c++11 -pedantic")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires clang 3.0 or greater.")
        endif ()
        ## -Wno-ignored-qualifiers and -Wno-overloaded-virtual silence warnings in OGRE headers
        set(CMAKE_CXX_FLAGS                "-Wall -Wextra -Wno-unused -Wno-ignored-qualifiers -Wno-overloaded-virtual -std=c++11 -pedantic -fPIC")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    elseif (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
        ## /wd4251 silences warnings in OGRE headers
        set(CMAKE_CXX_FLAGS                "/EHsc /wd4251 /D_CRT_SECURE_NO_WARNINGS")
        ##set(CMAKE_CXX_FLAGS_DEBUG          "")
        ##set(CMAKE_CXX_FLAGS_MINSIZEREL     "")
        ##set(CMAKE_CXX_FLAGS_RELEASE        "")
        ##set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 17.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 17.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires vs2012 or greater.")
        endif ()
    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
        set(CMAKE_CXX_FLAGS                "-Wall -Wextra -Wno-unused -std=c++11 -pedantic -fPIC ")
        set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
        if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 13.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 13.0))
            message(FATAL_ERROR "${PROJECT_NAME} requires icc 13 or greater.")
        endif ()
    else ()
        message(FATAL_ERROR "Your C++ compiler does not have the required C++11 support.")
    endif ()
    
    ## CMake defaults to leaving CMAKE_BUILD_TYPE empty. 
    ## This screws updifferentiation between debug and release builds.   
    if (CMAKE_BUILD_TYPE STREQUAL "")
      set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif ()
    
    ## Change default install prefix
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../bin" CACHE PATH "install prefix" FORCE)
    endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

    ##dlfcn-win32
    if(WIN32)
        set(USE_DLFCN_WIN32 FALSE CACHE BOOL "Use dlfcn-win32?" FORCE)
    endif()
    
    add_subdirectory(Base)
    add_subdirectory(External)
    
    add_subdirectory(Framework)
    list(APPEND SMOKE_LIBRARIES Framework)
    
    add_subdirectory(Systems)
   
    if(UNIX)
        set(USE_PRETTY_TRACE FALSE CACHE BOOL "Use backward-cpp?" FORCE)
        if(USE_PRETTY_TRACE)
            find_package(LibBfd)
            if (LIBBFD_FOUND)
                list(APPEND SMOKE_LIBRARIES ${LIBBFD_LIBRARIES})
                list(APPEND SMOKE_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIRS})
                add_definitions(-DBACKWARD_HAS_BFD=1)
                list(APPEND SMOKE_SOURCE ${CMAKE_SOURCE_DIR}/External/backward-cpp/backward.cpp)
            endif ()
        endif ()
    endif()
    
    list(APPEND SMOKE_SOURCE ${CMAKE_SOURCE_DIR}/Main.cpp)
    
    list(APPEND SMOKE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})

    include_directories(${SMOKE_INCLUDE_DIRS} )
    add_executable(Smoke ${SMOKE_SOURCE})
    target_link_libraries(Smoke ${SMOKE_LIBRARIES})
    
    install (TARGETS Smoke 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo)
    install (TARGETS Smoke 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug)

