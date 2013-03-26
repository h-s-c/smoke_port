# public domain

add_subdirectory(Ai)
add_subdirectory(AudioFMOD)
add_subdirectory(Explosion)
add_subdirectory(Geometry)
add_subdirectory(GraphicsOGRE)
add_subdirectory(InputOIS)
add_subdirectory(ProceduralTrees)
add_subdirectory(Water)

find_package(HAVOK)
if (HAVOK_FOUND)
    add_subdirectory(PhysicsHAVOK)
endif ()
