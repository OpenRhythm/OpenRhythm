cmake_minimum_required(VERSION 3.0)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
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
find_package(PNG REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/engine)
include_directories(${PROJECT_SOURCE_DIR}/src/game)
include_directories(${PROJECT_SOURCE_DIR}/src/gl)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(OSX_APP_BUNDLE 0)

if(NOT DEFINED ENV{NO_APP})
    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(OSX_APP_BUNDLE 1)
    endif()
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh2d.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/texture.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.hpp)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.cpp)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs)
set(ttvfs_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/ttvfs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSArchiveLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSBase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDebug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDebug.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDefines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDir.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDir.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDirInternal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDirInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDirView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSDirView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSFileFuncs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSFileFuncs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSRefcounted.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSRoot.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSRoot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSSystemPaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSSystemPaths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSTools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs/VFSTools.h
)
source_group("extern\\ttvfs" FILES ${ttvfs_SRC})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip)
set(ttvfs_zip_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSDirZip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSDirZip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSFileZip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSFileZip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSZipArchiveLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSZipArchiveLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSZipArchiveRef.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/VFSZipArchiveRef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/miniz.c
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/miniz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/ttvfs/ttvfs_zip/ttvfs_zip.h
)
source_group("extern\\ttvfs_zip" FILES ${ttvfs_zip_SRC})


include_directories(${PROJECT_SOURCE_DIR}/extern/png++)

source_group("extern\\png++" FILES ${PNGPP_SRC})

set( SOURCE ${CORE_HEADERS} ${CORE_SOURCE} ${GAME_SOURCE} ${GAME_HEADERS} ${ttvfs_SRC} ${ttvfs_zip_SRC} ${GL})

add_executable(musicgame ${SOURCE})

set(BINARY_DATA_DIR ${CMAKE_BINARY_DIR}/data)
set(SOURCE_DATA_DIR ${CMAKE_SOURCE_DIR}/data)

add_custom_command(TARGET musicgame PRE_BUILD
                  COMMAND ${CMAKE_COMMAND} -E make_directory
                  ${BINARY_DATA_DIR})
add_custom_command(TARGET musicgame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${SOURCE_DATA_DIR} $<TARGET_FILE_DIR:musicgame>/data/)

if(OSX_APP_BUNDLE)
    set_target_properties(musicgame PROPERTIES MACOSX_BUNDLE TRUE)
endif()

target_link_libraries(musicgame ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY})
