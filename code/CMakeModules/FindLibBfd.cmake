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
