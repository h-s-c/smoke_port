# public domain

# - Find PagedGeometry plugin

find_package(PkgConfig)

find_path(PG_INCLUDE_DIR PagedGeometry.h HINTS
    /usr/include/ogre/PagedGeometry
    /usr/include/OGRE/PagedGeometry
    /usr/local/include/ogre/PagedGeometry
    /usr/local/include/OGRE/PagedGeometry
    ${CMAKE_SOURCE_DIR}/External/PagedGeometry/include/PagedGeometry
    )

find_library(PG_LIBRARY NAMES PagedGeometry libPagedGeometry HINTS 
    /usr/lib${LIB_SUFFIX}/ogre
    /usr/lib${LIB_SUFFIX}/OGRE
    /usr/lib64/OGRE
    /usr/local/lib${LIB_SUFFIX}/ogre
    /usr/local/lib${LIB_SUFFIX}/OGRE
    ${CMAKE_SOURCE_DIR}/External/PagedGeometry/lib
    )

set(PG_LIBRARIES ${PG_LIBRARY} )
set(PG_INCLUDE_DIRS ${PG_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PagedGeometry  DEFAULT_MSG
                                  PG_LIBRARY PG_INCLUDE_DIR)

mark_as_advanced(PG_INCLUDE_DIR PG_LIBRARY )
