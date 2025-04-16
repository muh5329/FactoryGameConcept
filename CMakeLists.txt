cmake_minimum_required(VERSION 3.10)
project(FactoryGameConcept CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB SOURCES "*.cpp")  # now using .cpp

# Platform-specific configuration
if(APPLE)
    message(STATUS "Configuring for macOS")
    
    # Bullet Physics
    set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3/install/")
    include_directories(${BULLET_DIR})

    # Raylib
    set(RAYLIB_DIR "./include/")
    include_directories(${RAYLIB_DIR})

    add_executable(FactoryGameConcept ${SOURCES})

    target_link_directories(FactoryGameConcept PRIVATE lib/darwin/)

    target_link_libraries(FactoryGameConcept
        raylib
        BulletDynamics
        BulletCollision
        LinearMath
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
    )

else() # Linux
    message(STATUS "Configuring for Linux")

    # Bullet Physics
    set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3")
    include_directories(${BULLET_DIR}/src)
    link_directories(${BULLET_DIR}/build/lib)

    # Raylib
    set(RAYLIB_DIR "./include/")
    include_directories(${RAYLIB_DIR}/src)
    link_directories(${RAYLIB_DIR}/src)

    add_executable(FactoryGameConcept ${SOURCES})

    target_link_libraries(FactoryGameConcept
        raylib
        BulletDynamics
        BulletCollision
        LinearMath
        GL
        m
        pthread
        dl
        rt
        X11
    )
endif()
