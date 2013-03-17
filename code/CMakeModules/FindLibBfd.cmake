# public domain

# - Find libbfd library
# Find the native binutils headers and libraries.
#
#  LIBBFD_FOUND - system has libbfd
#  LIBBFD_INCLUDE_DIRS - the libbfd include directory
#  LIBBFD_LIBRARIES - Link these to use libbfd

# Look for the header file.
find_path(LIBBFD_INCLUDE_DIR NAMES bfd.h)
mark_as_advanced(LIBBFD_INCLUDE_DIR)

# Look for the library.
find_library(LIBBFD_LIBRARY NAMES bfd)
mark_as_advanced(LIBBFD_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set TINYXML_FOUND to TRUE if 
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(LibBfd DEFAULT_MSG LIBBFD_LIBRARY LIBBFD_INCLUDE_DIR)

set(LIBBFD_LIBRARIES ${LIBBFD_LIBRARY})
set(LIBBFD_INCLUDE_DIRS ${LIBBFD_INCLUDE_DIR})
