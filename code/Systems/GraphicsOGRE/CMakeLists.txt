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

project (SystemGraphicsOGRE)
    set( SYSTEM_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicBuffer.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicLines.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/DynamicRenderable.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/MovableText.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/NormalizedLines.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Object.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectCamera.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectChart.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectCPUChart.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectLight.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectLightFire.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectMesh.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectMeshAnimated.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectPagedGeometryLayer.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectParticles.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectParticleSystem.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectStatWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/ObjectWorkloadWindow.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Scene.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/System.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/SystemGraphicsOGRE.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Task.cpp
    )
    list(APPEND SYSTEM_SOURCE ${SYSTEM_SOURCE})

if(UNIX)  
    ## cg plugin is often not part of linux packages
    set( CG_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/CgFxScriptLoader.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/CgPlugin.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/CgProgram.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/CgProgramError.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/CgProgramFactory.cpp
    )
    list(APPEND SYSTEM_SOURCE ${CG_SOURCE})
endif()

    set( PG_SOURCE
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/BatchedGeometry.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/BatchPage.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/GrassLoader.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/ImpostorPage.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/PagedGeometry.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/PropertyMaps.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/StaticBillboardSet.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/TreeLoader2D.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/TreeLoader3D.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/WindBatchedGeometry.cpp
        ${CMAKE_SOURCE_DIR}/Systems/GraphicsOGRE/Extras/WindBatchPage.cpp
    )
    list(APPEND SYSTEM_SOURCE ${PG_SOURCE})
    
    ##ogre
    find_package(OGRE REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_INCLUDE_DIR})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_INCLUDE_DIR}/../../boost)
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_RenderSystem_GL_INCLUDE_DIRS})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_Plugin_ParticleFX_INCLUDE_DIRS})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_Overlay_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_LIBRARIES}) 
    list(APPEND SYSTEM_LIBRARIES ${OGRE_RenderSystem_GL_LIBRARIES})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_Plugin_ParticleFX_LIBRARIES})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_Overlay_LIBRARIES})
if(WIN32)  
    ##list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_RenderSystem_Direct3D9_INCLUDE_DIRS})
    ##list(APPEND SYSTEM_LIBRARIES ${OGRE_RenderSystem_Direct3D9_LIBRARIES})
    list(APPEND SYSTEM_INCLUDE_DIRS ${OGRE_Plugin_CgProgramManager_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${OGRE_Plugin_CgProgramManager_LIBRARIES})
	set(Boost_ADDITIONAL_VERSIONS "1.54" "1.54.0" "1.53" "1.53.0" "1.52" "1.52.0" "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40")
	set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/External/ogre/boost")
	set(Boost_USE_STATIC_LIBS TRUE)
    find_package(Boost COMPONENTS thread system date_time chrono QUIET)
    list(APPEND SYSTEM_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${Boost_LIBRARIES})
endif()
if(UNIX)  
    ## cg
    find_package(Cg REQUIRED)
    list(APPEND SYSTEM_INCLUDE_DIRS ${Cg_INCLUDE_DIRS})
    list(APPEND SYSTEM_LIBRARIES ${Cg_LIBRARIES}) 
endif()
    
     ##base
    list(APPEND SYSTEM_LIBRARIES Base)

    list(APPEND SYSTEM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    
    include_directories(${SYSTEM_INCLUDE_DIRS})
    add_library (SystemGraphicsOGRE SHARED ${SYSTEM_SOURCE})
    target_link_libraries(SystemGraphicsOGRE ${SYSTEM_LIBRARIES})
    set_target_properties(SystemGraphicsOGRE PROPERTIES PREFIX "")
    set_target_properties(SystemGraphicsOGRE PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    install (TARGETS SystemGraphicsOGRE 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS SystemGraphicsOGRE 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
        
if(WIN32)    
    install(FILES
        ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
        ${OGRE_PLUGIN_DIR_REL}/OgreOverlay.dll
        ${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
        ##${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
        ${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
        ${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release
        CONFIGURATIONS Release RelWithDebInfo
    )
    install(FILES
        ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/OgreOverlay_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
        ##${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
        ${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug
        CONFIGURATIONS Debug
    )
endif()
