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
        ${CMAKE_SOURCE_DIR}/Framework/TaskManagerTP.cpp
        ${CMAKE_SOURCE_DIR}/Framework/Universal.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${FRAMEWORK_SOURCE})

    ##tinyxml
    find_package( TinyXML REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${TINYXML_LIBRARIES})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${TINYXML_INCLUDE_DIRS})

    ##boost
    find_package( Boost COMPONENTS filesystem system REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${Boost_LIBRARIES})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
    list(APPEND FRAMEWORK_LIBRARY_DIRS ${Boost_LIBRARY_DIR})
   
    if (UNIX) 
        ##pthreads
        find_package(Threads)
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
        ##dlfcn
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_DL_LIBS})
    else ()
        list(APPEND FRAMEWORK_SOURCE
            ${CMAKE_SOURCE_DIR}/External/dlfcn-win32/dlfcn.c
        )
        list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/External/dlfcn-win32)
    endif () 
   
    ##base
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
