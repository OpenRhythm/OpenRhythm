cmake_minimum_required(VERSION 3.0)
project(musicgame)

set(PL_WINDOWS 1)
set(PL_LINUX 2)
set(PL_OSX 3)

set(PLATFORM 0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Building for Windows.")
    set(ENV{SDL2DIR} "${PROJECT_SOURCE_DIR}/extern/sdl2")
    set(ENV{FREETYPE_DIR} "${PROJECT_SOURCE_DIR}/extern/freetype")
    set(ENV{GLM_ROOT_DIR} "${PROJECT_SOURCE_DIR}/extern/glm")
    set(PLATFORM PL_WINDOWS)

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Building for Linux.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(PLATFORM PL_LINUX)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Building for Mac OS X.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
    set(PLATFORM PL_OSX)

endif()

find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/src/core/vfs)
include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/engine)
include_directories(${PROJECT_SOURCE_DIR}/src/game)
include_directories(${PROJECT_SOURCE_DIR}/src/gl)
include_directories(${PROJECT_SOURCE_DIR}/src)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
  "${PROJECT_SOURCE_DIR}/src/config.hpp"
)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/files.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSArchiveLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSBase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDefines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDir.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirZip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFileFuncs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFileZip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSRefcounted.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSRoot.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSSystemPaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSTools.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSZipArchiveLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSZipArchiveRef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/miniz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/ttvfs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/ttvfs_zip.h)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/files.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDir.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirInternal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSDirZip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFileFuncs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSFileZip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSRoot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSSystemPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSTools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSZipArchiveLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/VFSZipArchiveRef.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs/miniz.c)

if(INC_PATH)
    string(REPLACE ":" ";" _INC_PATH ${INC_PATH})
    foreach(PATH ${_INC_PATH})
        include_directories(${PATH})
    endforeach()
endif()

source_group("src\\core" FILES ${CORE_SOURCE} ${CORE_HEADERS})


set(GAME_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.cpp)
set(GAME_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.hpp)

source_group("src\\game" FILES ${GAME_SOURCE} ${GAME_HEADERS})

set(GL ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_core32.h
       ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_core32.c
       ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl.hpp)

source_group("src\\gl" FILES ${GL})


set( SOURCE ${CORE_HEADERS} ${CORE_SOURCE} ${GAME_SOURCE} ${GL})

add_executable(musicgame ${SOURCE})

if(NOT DEFINED ENV{NO_APP})
    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set_target_properties(musicgame PROPERTIES MACOSX_BUNDLE TRUE)
    endif()
endif()

target_link_libraries(musicgame ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${FREETYPE_LIBRARIES} ${ADDITIONAL_LIBS})
