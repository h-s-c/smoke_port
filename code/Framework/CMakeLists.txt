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

    ##boost.filesystem
    find_package( Boost COMPONENTS filesystem system REQUIRED)
    list(APPEND FRAMEWORK_LIBRARIES ${Boost_LIBRARIES})
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
    list(APPEND FRAMEWORK_LIBRARY_DIRS ${Boost_LIBRARY_DIR})

    ##pthreads
    if (UNIX) 
        find_package(Threads)
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    endif ()

    ##dl
    if (UNIX) 
        ##find_package(DL)
        list(APPEND FRAMEWORK_LIBRARIES ${CMAKE_DL_LIBS})
    endif ()
    
    ##base
    list(APPEND FRAMEWORK_LIBRARIES Base)
    
    list(APPEND FRAMEWORK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${FRAMEWORK_INCLUDE_DIRS} )
    link_directories(${FRAMEWORK_LIBRARY_DIRS} )
    add_library (Framework ${FRAMEWORK_SOURCE})
    target_link_libraries(Framework ${FRAMEWORK_LIBRARIES})
