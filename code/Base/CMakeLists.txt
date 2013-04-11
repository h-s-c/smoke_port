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

project (Base)
    set( BASE_SOURCE ${CMAKE_SOURCE_DIR}/Base/Library.cpp )
    set( BASE_SOURCE ${CMAKE_SOURCE_DIR}/Base/Math.cpp )
    list(APPEND BASE_SOURCE ${BASE_SOURCE})
    list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    if (UNIX) 
        ##dlfcn
        list(APPEND BASE_LIBRARIES ${CMAKE_DL_LIBS})
    else (MSVC AND USE_DLFCN_WIN32) 
        ##dlfcn-win32
        add_definitions(-DUSE_DLFCN_WIN32)
        list(APPEND BASE_SOURCE ${CMAKE_SOURCE_DIR}/External/dlfcn-win32/dlfcn.c)
        list(APPEND BASE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/External/dlfcn-win32)
    endif () 
    
    include_directories(${BASE_INCLUDE_DIRS} )
    add_library (Base ${BASE_SOURCE})
    target_link_libraries(Base ${BASE_LIBRARIES})
    set_target_properties(Base PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
