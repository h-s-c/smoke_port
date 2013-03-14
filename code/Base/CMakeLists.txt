# public domain

project (Base)
    set( BASE_SOURCE
        ${CMAKE_SOURCE_DIR}/Base/Math.cpp
        ${CMAKE_SOURCE_DIR}/Base/MathX.cpp
        ${CMAKE_SOURCE_DIR}/Base/UnitAllocator.cpp
    )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
