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
    set( TINYXML_SOURCE
        ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxml.cpp
        ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlerror.cpp
        ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlparser.cpp
        ${CMAKE_SOURCE_DIR}/External/tinyxml/tinystr.cpp
    )
    list(APPEND FRAMEWORK_SOURCE ${TINYXML_SOURCE})

    ##diy
    list(APPEND FRAMEWORK_INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/External/diy)
    list(APPEND FRAMEWORK_LIBRARIES diy)
    
    ##stdnext
    list(APPEND FRAMEWORK_INCLUDE_DIRS  ${CMAKE_SOURCE_DIR}/External/stdnext)
    list(APPEND FRAMEWORK_LIBRARIES stdnext)
   
    if (UNIX) 
        ##pthreads
        find_package(Threads)
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    endif ()   
   
    ##base
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
