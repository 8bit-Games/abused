# Android CMake configuration for Abused Game Engine

cmake_minimum_required(VERSION 3.18.1)

project(abused-engine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to game engine source (adjust relative path as needed)
set(ABUSED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../src)

# SDL2 paths (from setup script)
set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jni/SDL2)
set(SDL2_MIXER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jni/SDL2_mixer)

# Include directories
include_directories(
    ${ABUSED_SRC_DIR}
    ${ABUSED_SRC_DIR}/lisp
    ${ABUSED_SRC_DIR}/imlib
    ${ABUSED_SRC_DIR}/net
    ${ABUSED_SRC_DIR}/sdlport
    ${ABUSED_SRC_DIR}/mobile
    ${ABUSED_SRC_DIR}/lol
    ${SDL2_DIR}/include
    ${SDL2_MIXER_DIR}/include
)

# Collect all game engine source files
file(GLOB_RECURSE LISP_SOURCES ${ABUSED_SRC_DIR}/lisp/*.cpp)
file(GLOB_RECURSE IMLIB_SOURCES ${ABUSED_SRC_DIR}/imlib/*.cpp)
file(GLOB_RECURSE NET_SOURCES ${ABUSED_SRC_DIR}/net/*.cpp)
file(GLOB_RECURSE SDLPORT_SOURCES ${ABUSED_SRC_DIR}/sdlport/*.cpp)
file(GLOB_RECURSE LOL_SOURCES ${ABUSED_SRC_DIR}/lol/*.cpp)
file(GLOB MOBILE_SOURCES ${ABUSED_SRC_DIR}/mobile/*.cpp)

# Main game engine sources
set(ENGINE_SOURCES
    ${ABUSED_SRC_DIR}/game.cpp
    ${ABUSED_SRC_DIR}/level.cpp
    ${ABUSED_SRC_DIR}/objects.cpp
    ${ABUSED_SRC_DIR}/view.cpp
    ${ABUSED_SRC_DIR}/cache.cpp
    ${ABUSED_SRC_DIR}/particle.cpp
    ${ABUSED_SRC_DIR}/collide.cpp
    ${ABUSED_SRC_DIR}/ability.cpp
    ${ABUSED_SRC_DIR}/items.cpp
    ${ABUSED_SRC_DIR}/dev.cpp
    ${ABUSED_SRC_DIR}/chars.cpp
    ${ABUSED_SRC_DIR}/automap.cpp
    ${ABUSED_SRC_DIR}/help.cpp
    ${ABUSED_SRC_DIR}/intsect.cpp
    ${ABUSED_SRC_DIR}/loader2.cpp
    ${ABUSED_SRC_DIR}/seq.cpp
    ${ABUSED_SRC_DIR}/points.cpp
    ${ABUSED_SRC_DIR}/morpher.cpp
    ${ABUSED_SRC_DIR}/menu.cpp
    ${ABUSED_SRC_DIR}/director.cpp
    ${ABUSED_SRC_DIR}/configuration.cpp
    ${ABUSED_SRC_DIR}/light.cpp
    ${ABUSED_SRC_DIR}/devsel.cpp
    ${ABUSED_SRC_DIR}/crc.cpp
    ${ABUSED_SRC_DIR}/gamma.cpp
    ${ABUSED_SRC_DIR}/specache.cpp
    ${ABUSED_SRC_DIR}/netcfg.cpp
    ${ABUSED_SRC_DIR}/innet.cpp
    ${ABUSED_SRC_DIR}/chat.cpp
    ${ABUSED_SRC_DIR}/endgame.cpp
    ${ABUSED_SRC_DIR}/loadgame.cpp
    ${ABUSED_SRC_DIR}/profile.cpp
    ${ABUSED_SRC_DIR}/cop.cpp
    ${ABUSED_SRC_DIR}/statbar.cpp
    ${ABUSED_SRC_DIR}/compiled.cpp
    ${ABUSED_SRC_DIR}/ant.cpp
    ${ABUSED_SRC_DIR}/sensor.cpp
    ${ABUSED_SRC_DIR}/demo.cpp
    ${ABUSED_SRC_DIR}/lcache.cpp
    ${ABUSED_SRC_DIR}/nfclient.cpp
    ${ABUSED_SRC_DIR}/clisp.cpp
    ${ABUSED_SRC_DIR}/gui.cpp
    ${ABUSED_SRC_DIR}/transp.cpp
    ${ABUSED_SRC_DIR}/property.cpp
    ${ABUSED_SRC_DIR}/extend.cpp
    ${ABUSED_SRC_DIR}/console.cpp
    ${ABUSED_SRC_DIR}/smallfnt.cpp
    ${ABUSED_SRC_DIR}/fnt6x13.cpp
    ${ABUSED_SRC_DIR}/ui/volumewindow.cpp
)

# JNI bridge source
set(BRIDGE_SOURCES
    src/main/cpp/abused-bridge.cpp
)

# Create shared library
add_library(abused-engine SHARED
    ${ENGINE_SOURCES}
    ${LISP_SOURCES}
    ${IMLIB_SOURCES}
    ${NET_SOURCES}
    ${SDLPORT_SOURCES}
    ${LOL_SOURCES}
    ${MOBILE_SOURCES}
    ${BRIDGE_SOURCES}
)

# Add SDL2 as a subdirectory (builds from source)
if(EXISTS ${SDL2_DIR}/CMakeLists.txt)
    add_subdirectory(${SDL2_DIR} SDL2)
    set(SDL2_AVAILABLE TRUE)
else()
    message(WARNING "SDL2 not found at ${SDL2_DIR}. Run setup-sdl2-android.sh")
    set(SDL2_AVAILABLE FALSE)
endif()

# Add SDL2_mixer as a subdirectory
if(EXISTS ${SDL2_MIXER_DIR}/CMakeLists.txt)
    add_subdirectory(${SDL2_MIXER_DIR} SDL2_mixer)
    set(SDL2_MIXER_AVAILABLE TRUE)
else()
    message(WARNING "SDL2_mixer not found at ${SDL2_MIXER_DIR}. Run setup-sdl2-android.sh")
    set(SDL2_MIXER_AVAILABLE FALSE)
endif()

# Find Android system libraries
find_library(GLES_LIBRARY GLESv2)
find_library(LOG_LIBRARY log)
find_library(ANDROID_LIBRARY android)
find_library(OPENSLES_LIBRARY OpenSLES)

# Link libraries
target_link_libraries(abused-engine
    ${GLES_LIBRARY}
    ${LOG_LIBRARY}
    ${ANDROID_LIBRARY}
    ${OPENSLES_LIBRARY}
)

# Link SDL2 if available
if(SDL2_AVAILABLE)
    target_link_libraries(abused-engine SDL2::SDL2)
endif()

if(SDL2_MIXER_AVAILABLE)
    target_link_libraries(abused-engine SDL2_mixer::SDL2_mixer)
endif()

# Compiler flags
target_compile_options(abused-engine PRIVATE
    -Wall
    -Wno-unused-parameter
    -Wno-unused-variable
    -DHAVE_CONFIG_H
    -DPACKAGE_NAME="abused"
    -DPACKAGE_VERSION="1.0.0"
)
