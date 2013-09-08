# Copyright (c) 2004-2008, Adrian Boeing.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are permitted 
# provided that the following conditions are met: Redistributions of source code must retain the 
# above copyright notice, this list of conditions and the following disclaimer. Redistributions in 
# binary form must reproduce the above copyright notice, this list of conditions and the following 
# disclaimer in the documentation and/or other materials provided with the distribution.
# Neither the name of the Physics Abstraction Layer nor the names of its contributors may be used to 
# endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Locate Havok
# This module defines XXX_FOUND, XXX_INCLUDE_DIRS and XXX_LIBRARIES standard variables

INCLUDE(FindPackageTargetLibraries)

FIND_PATH(HAVOK_INCLUDE_DIR Common/Base/hkBase.h
    HINTS
    $ENV{HAVOK_DIR}
    $ENV{HAVOK_PATH}
    ${ADDITIONAL_SEARCH_PATHS}
    PATHS
        ${CMAKE_SOURCE_DIR}/External/hk2013_1_0_r1/Source
)
mark_as_advanced(HAVOK_INCLUDE_DIR)

SET(HAVOK_LIBS 
    hkaAnimation 
    hkaInternal 
	hkaPhysics2012Bridge
    hkBase
    hkcdCollide
    hkcdInternal
    hkCompat
    hkgBridge
    hkgCommon
    hkgDx11
    hkGeometryUtilities
    hkgOglES
    hkgOglES2
    hkgSoundCommon
    hkgSoundXAudio2
    hkInternal
    hkpCollide
    hkpConstraint
    hkpConstraintSolver
    hkpDynamics
    hkpInternal
    hkpUtilities
    hkpVehicle
    hkSceneData
    hkSerialize
    hkVisualize
    )
SET(HAVOK_LIBRARIES)

FOREACH(CUR_LIB ${HAVOK_LIBS})
    STRING(TOLOWER "${CUR_LIB}" CUR_LIB_LOWER)
    FIND_LIBRARY(HAVOK_LIBRARY_${CUR_LIB}
        NAMES ${CUR_LIB} ${CUR_LIB_LOWER}
        HINTS
            $ENV{HAVOK_DIR}
            $ENV{HAVOK_PATH}
            ${ADDITIONAL_SEARCH_PATHS}
        PATH_SUFFIXES lib Lib lib64 "Lib/win32_vs2012_win8/release_dll"
        PATHS
            ${CMAKE_SOURCE_DIR}/External/hk2013_1_0_r1
    )
    mark_as_advanced(HAVOK_LIBRARY_${CUR_LIB})

    FIND_LIBRARY(HAVOK_LIBRARY_${CUR_LIB}_DEBUG
        NAMES ${CUR_LIB} ${CUR_LIB_LOWER} "${CUR_LIB}d" "${CUR_LIB_LOWER}d" "${CUR_LIB}_d" "${CUR_LIB_LOWER}_d"
        HINTS
            $ENV{HAVOK_DIR}
            $ENV{HAVOK_PATH}
            ${ADDITIONAL_SEARCH_PATHS}
        PATH_SUFFIXES lib Lib lib64 "Lib/win32_vs2012_win8/debug_dll"
        PATHS
            ${CMAKE_SOURCE_DIR}/External/hk2013_1_0_r1
    )
    mark_as_advanced(HAVOK_LIBRARY_${CUR_LIB}_DEBUG)

    # Combine all libs to one variable
    IF(HAVOK_LIBRARY_${CUR_LIB})
        FIND_PACKAGE_ADD_TARGET_LIBRARIES(HAVOK "${HAVOK_LIBRARY_${CUR_LIB}}" "${HAVOK_LIBRARY_${CUR_LIB}_DEBUG}")
    ENDIF()
ENDFOREACH()


# handle the QUIETLY and REQUIRED arguments and set HAVOK_FOUND to TRUE if 
# all listed variables are TRUE
SET(HAVOK_LIBRARY_FULL_LIST)
FOREACH(CUR_LIB ${HAVOK_LIBS})
    LIST(APPEND HAVOK_LIBRARY_FULL_LIST "HAVOK_LIBRARY_${CUR_LIB}")
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HAVOK DEFAULT_MSG ${HAVOK_LIBRARY_FULL_LIST} HAVOK_INCLUDE_DIR)

IF(HAVOK_FOUND)
    # HAVOK_LIBRARIES has been set before
    SET(HAVOK_INCLUDE_DIRS ${HAVOK_INCLUDE_DIR})
ELSE()
    SET(HAVOK_LIBRARIES)
    SET(HAVOK_INCLUDE_DIRS)
ENDIF()
