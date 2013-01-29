# - Locate FMOD library
# This module defines
#  FMOD_LIBRARY, the library to link against
#  FMOD_FOUND, if false, do not try to link to FMOD
#  FMOD_INCLUDE_DIR, where to find headers.

IF(FMOD_LIBRARY AND FMOD_INCLUDE_DIR)
  # in cache already
  SET(FMOD_FIND_QUIETLY TRUE)
ENDIF(FMOD_LIBRARY AND FMOD_INCLUDE_DIR)


FIND_PATH(FMOD_INCLUDE_DIR
  fmod.hpp
  PATHS
  $ENV{FMODE_DIR}/include
  /opt/fmod/api/inc
  "${PROGRAMFILESX86}/FMOD SoundSystem/FMOD Programmers API Win32/api/inc"
  PATH_SUFFIXES fmod
)

set(WINLIBNAME " ")
set(WINLIBEXT " ")

IF(WIN32 AND NOT UNIX)
    IF (MSVC)
        set(WINLIBNAME fmodex_vc)
        set(WINLIBEXT "lib")
    ENDIF(MSVC)
    IF(CMAKE_GENERATOR MATCHES "Borland Makefiles")
        set(WINLIBNAME fmodex_bc)
        set(WINLIBEXT "lib")
    ENDIF()
    IF(CMAKE_GENERATOR MATCHES "MinGW Makefiles")
        set(WINLIBNAME libfmodex)
        set(WINLIBEXT "a")
    ENDIF()
ENDIF(WIN32 AND NOT UNIX)

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LINUXLIBS libfmodex64)
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(LINUXLIBS libfmodex)
ENDIF()

IF(UNIX AND NOT WIN32)
    set(LINUXADDLIBS "")
    set(VERSIONSMOD "")
    set(VERSIONS "")

    set (PASTVERSIONS 4.28.08 4.28.07 4.28.06 4.28.05 4.28.04 4.28.03 4.28.02 4.28.01 4.28.00 4.27.10 4.27.09 4.27.08 4.27.07 4.27.06 4.27.05 4.27.04 4.27.03 4.27.02 4.27.01 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.27.00 4.25.00 4.25.00 4.25.00 4.25.00 4.25.00 4.25.00 4.25.00 4.25.00 4.25.00 4.23.00 4.23.00 4.23.00 4.23.00 4.23.00 4.23.00 4.23.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.21.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.19.00 4.17.00 4.17.00 4.17.00 4.17.00 4.17.00 4.17.00 4.17.00 4.17.00 4.15.00 4.13.00 4.11.00 4.09.07 4.09.06 4.09.05 4.09.04 4.09.03 4.09.02 4.09.01 4.08.00 4.07.25 4.07.24 4.07.23 4.07.22 4.07.21 4.07.20 4.07.19 4.07.18 4.07.17 4.07.16 4.07.15 4.07.14 4.07.13 4.07.12 4.07.11 4.07.10 4.07.09 4.07.08 4.07.07 4.07.06 4.07.05 4.07.04 4.07.03 4.07.02 4.07.01 4.07.00 4.06.00 4.04.53 4.04.51 4.04.49 4.04.47 4.04.45 4.04.43 4.04.41 4.04.39 4.04.37 4.04.35 4.04.33 4.04.31 4.04.29 4.04.28 4.04.27 4.04.26 4.04.25 4.04.24 4.04.23 4.04.21 4.04.20 4.04.19 4.04.18 4.04.17 4.04.16 4.04.15 4.04.14 4.04.13 4.04.12 4.04.11 4.04.10 4.04.08 4.04.06 4.04.04 4.04.02 4.04.00 4.03.10 4.03.09 4.03.08 4.03.07 4.03.06 4.03.05 4.03.04 4.03.03 4.03.02 4.03.01 4.03.00 4.02.10 4.02.09 4.02.08 4.02.07 4.02.06 4.02.05 4.02.04 4.02.03 4.02.02 4.02.01 4.02.00 4.01.07 4.01.06 4.01.05 4.01.04 4.01.03 4.01.02 4.01.01 4.01.00 4.00.37 4.00.36 4.00.35 4.00.34 4.00.33 4.00.32 4.00.31 4.00.30 4.00.29 4.00.28 4.00.27 4.00.26 4.00.25 4.00.24 4.00.23 4.00.22 4.00.21 4.00.20 4.00.19 4.00.18 4.00.17 4.00.16 4.00.14 4.00.13 4.00.12 4.00.11 4.00.10 4.00.09 4.00.08 4.00.07 4.00.06 4.00.05 4.00.04 4.00.03 4.00.02 4.00.01)

    #add existing supported versions
    FOREACH (CURRENTVERSION ${PASTVERSIONS})
        set(LINUXADDLIBS ${LINUXADDLIBS} ${LINUXLIBS}-${CURRENTVERSION}.so)
    ENDFOREACH(CURRENTVERSION)

    #add a short range of future versions 
    #left extra logic in the iterators so the numbers can be changed and versions can start with more major versions
    set(LINUXADDLIBS ${LINUXADDLIBS} ${LINUXLIBS}-${ITERATOR}.so)

    set(LINUXADDLIBS ${LINUXADDLIBS} ${LINUXLIBS}-4.so)
    set(VERSIONS ${VERSIONS} 4)
    FOREACH (CURRENTVERSION ${VERSIONS})
        FOREACH (ITERATOR RANGE 28 50)
        set(ADDITIONAL "")
        IF (${ITERATOR} LESS 10)
        set(ADDITIONAL 0)
        ENDIF()
        set(LINUXADDLIBS ${LINUXADDLIBS} ${LINUXLIBS}-${CURRENTVERSION}.${ADDITIONAL}${ITERATOR}.so)
        set(VERSIONSMOD ${VERSIONSMOD} ${CURRENTVERSION}.${ADDITIONAL}${ITERATOR})
        ENDFOREACH(ITERATOR)
    ENDFOREACH(CURRENTVERSION)
    set(VERSIONS ${VERSIONS} ${VERSIONSMOD})
    FOREACH (CURRENTVERSION ${VERSIONS})
        FOREACH (ITERATOR RANGE 0 30)
        set(ADDITIONAL "")
        IF (${ITERATOR} LESS 10)
        set(ADDITIONAL 0)
        ENDIF()
        set(LINUXADDLIBS ${LINUXADDLIBS} ${LINUXLIBS}-${CURRENTVERSION}.${ADDITIONAL}${ITERATOR}.so)
        set(VERSIONSMOD ${VERSIONSMOD} ${CURRENTVERSION}.${ADDITIONAL}${ITERATOR})
        ENDFOREACH(ITERATOR)
    ENDFOREACH(CURRENTVERSION)
    set(VERSIONS ${VERSIONS} ${VERSIONSMOD})
    set(LINUXLIBS ${LINUXLIBS} ${LINUXADDLIBS})
ENDIF(UNIX AND NOT WIN32)

FIND_LIBRARY(FMOD_LIBRARY
  NAMES ${LINUXLIBS} ${WINLIBNAME}
  PATHS
  $ENV{FMOD_DIR}/api/lib
  /opt/fmod/api/lib
  "${PROGRAMFILESX86}/FMOD SoundSystem/FMOD Programmers API Win32/api/lib"
)

IF(FMOD_LIBRARY AND FMOD_INCLUDE_DIR)
  SET(FMOD_FOUND "YES")
  IF(NOT FMOD_FIND_QUIETLY)
    MESSAGE(STATUS "Found FMOD: ${FMOD_LIBRARY}")
  ENDIF(NOT FMOD_FIND_QUIETLY)
ELSE(FMOD_LIBRARY AND FMOD_INCLUDE_DIR)
  IF(NOT FMOD_FIND_QUIETLY)
    MESSAGE(STATUS "Warning: Unable to find FMOD!")
  ENDIF(NOT FMOD_FIND_QUIETLY)
ENDIF(FMOD_LIBRARY AND FMOD_INCLUDE_DIR)





