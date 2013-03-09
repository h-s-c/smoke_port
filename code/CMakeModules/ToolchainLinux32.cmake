# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER gcc -m32 -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2)
set(CMAKE_CXX_COMPILER g++ -m32 -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2)

set(CMAKE_SYSTEM_PROCESSOR i686)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH  /usr)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) 
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) 
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_LIBRARY_USRDIRES lib) 

