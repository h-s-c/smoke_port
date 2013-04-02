# public domain

project (SystemProceduralTrees)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/SystemProceduralTrees.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Task.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/Branch.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/Canopy.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/FatTreeGrammar.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/Observer.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/OgreBranch.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/OgreTree.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/PlainTreeGrammar.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/ScratchTreeGrammar.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/Segment.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/SpeciesGrammar.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/SpikeTreeGrammar.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/Tree.cpp
        ${CMAKE_SOURCE_DIR}/Systems/ProceduralTrees/Trees/TreeGrammar1.cpp

    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})
    
    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    if (UNIX) 
    ##tinyxml
		find_package( TinyXML REQUIRED)
		list(APPEND SYSTEM_LIBRARIES ${TINYXML_LIBRARIES})
		list(APPEND SYSTEM_INCLUDE_DIRS ${TINYXML_INCLUDE_DIRS})
    else (MSVC)
		##tinyxml
        list(APPEND SYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinystr.cpp)
        list(APPEND SYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxml.cpp)
        list(APPEND SYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlerror.cpp)
        list(APPEND SYSTEM_SOURCE ${CMAKE_SOURCE_DIR}/External/tinyxml/tinyxmlparser.cpp)
        list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/External/tinyxml)		
    endif () 
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)
    
    include_directories(${SYSTEM_INCLUDE_DIRS} )
    add_library (SystemProceduralTrees SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemProceduralTrees ${SYSTEM_LIBRARIES})
    set_target_properties(SystemProceduralTrees PROPERTIES PREFIX "")
    
    install (TARGETS SystemProceduralTrees
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemProceduralTrees
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
