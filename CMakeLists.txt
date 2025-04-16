cmake_minimum_required(VERSION 3.10)
project(YourProject C)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Source files
file(GLOB SOURCES "*.c")

# Source files
# set(SOURCES
#     game2.c
#     # Add other source files here
# )

# Platform-specific configuration
if(APPLE)
    message(STATUS "Configuring for macOS")
    
    # Bullet Physics setup
    set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3/src")
    include_directories(${BULLET_DIR}/src)

    # Raylib (macOS framework-style linking)
    set(RAYLIB_DIR "include/")
    include_directories(${RAYLIB_DIR}/src)

    add_executable(YourProject ${SOURCES})

    target_link_directories(YourProject PRIVATE lib/darwin/)
    
    target_link_libraries(YourProject
        raylib
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
    )

else() # Assume Linux
    message(STATUS "Configuring for Linux")

    # Bullet Physics setup
    set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3/src")
    include_directories(${BULLET_DIR}/src)
    link_directories(${BULLET_DIR}/lib)

    # Raylib setup
    set(RAYLIB_DIR "include/")
    include_directories(${RAYLIB_DIR}/src)
    link_directories(${RAYLIB_DIR}/src)

    add_executable(YourProject ${SOURCES})

    target_link_libraries(YourProject
        raylib
        GL
        m
        pthread
        dl
        rt
        X11
        BulletDynamics
        BulletCollision
        LinearMath
    )
endif()
