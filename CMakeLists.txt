cmake_minimum_required(VERSION 3.10)
project(FactoryGameConcept CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Recursively get all .cpp files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Include directory for headers (e.g., raylib.h, raymath.h)
include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Bullet Physics
# CHANGE THIS TO YOUR SOURCE DIRECTORY for your Bullet Install
set(BULLET_DIR "/Users/muneebhaq/Documents/projects/c_lib/bullet3/install/")
include_directories(${BULLET_DIR}/include/bullet) 
link_directories(${BULLET_DIR}/lib)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Platform-specific linking
if(APPLE)
    message(STATUS "Configuring for macOS")

    # Link Raylib statically (adjust name if needed)
    target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/darwin)

    target_link_libraries(${PROJECT_NAME}
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

elseif(UNIX)
    message(STATUS "Configuring for Linux")

    target_link_libraries(${PROJECT_NAME}
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
