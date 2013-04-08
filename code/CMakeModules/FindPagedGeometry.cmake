# public domain

# - Find PagedGeometry plugin

find_package(PkgConfig)

find_path(PG_INCLUDE_DIR PagedGeometry.h HINTS
    /usr/include/ogre/PagedGeometry
    /usr/include/OGRE/PagedGeometry
    /usr/local/include/ogre/PagedGeometry
    /usr/local/include/OGRE/PagedGeometry
    ${CMAKE_SOURCE_DIR}/External/ogre-paged/include/PagedGeometry
    )
    
find_library(PG_LIBRARY_RELEASE NAMES PagedGeometry libPagedGeometry HINTS 
    /usr/lib${LIB_SUFFIX}/ogre
    /usr/lib${LIB_SUFFIX}/OGRE
    /usr/lib64/OGRE
    /usr/local/lib${LIB_SUFFIX}/ogre
    /usr/local/lib${LIB_SUFFIX}/OGRE
    ${CMAKE_SOURCE_DIR}/External/ogre-paged/lib
    )
    
if(WIN32)     
find_library(PG_LIBRARY_DEBUG NAMES PagedGeometry_d libPagedGeometry_d HINTS 
    /usr/lib${LIB_SUFFIX}/ogre
    /usr/lib${LIB_SUFFIX}/OGRE
    /usr/lib64/OGRE
    /usr/local/lib${LIB_SUFFIX}/ogre
    /usr/local/lib${LIB_SUFFIX}/OGRE
    ${CMAKE_SOURCE_DIR}/External/ogre-paged/lib
    )

set(PG_LIBRARIES 
    debug ${PG_LIBRARY_DEBUG}
    optimized ${PG_LIBRARY_RELEASE}
    )
endif()

if(UNIX) 
set(PG_LIBRARIES 
    optimized ${PG_LIBRARY_RELEASE}
    )
endif()

set(PG_INCLUDE_DIRS ${PG_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PagedGeometry  DEFAULT_MSG
                                  PG_LIBRARIES PG_INCLUDE_DIR)

mark_as_advanced(PG_INCLUDE_DIR PG_LIBRARIES PG_LIBRARY_DEBUG PG_LIBRARY_RELEASE)
