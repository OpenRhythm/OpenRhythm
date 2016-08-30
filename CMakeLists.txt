####################################################################
#   Project configuration
####################################################################
cmake_minimum_required(VERSION 3.0)

project (OpenRhythm)
set     (OpenRhythm_VERSION_MAJOR 0)
set     (OpenRhythm_VERSION_MINOR 1)

set(APPLE_APP_NAME "openrhythm.app")
set(LINUX_HOMEPATH ".openrhythm")

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
####################################################################
#   C++14 / C11 support and flags
####################################################################
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
add_definitions ("-Wall")

####################################################################
#   Platform detection and rules
####################################################################
set(PL_DEFAULT  0)
set(PL_WINDOWS  1)
set(PL_LINUX    2)
set(PL_OSX      3)

set(OSX_APP_BUNDLE 0)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Building for Windows.")
    set(PLATFORM PL_WINDOWS)

    set(ENV{FREETYPE_DIR}   "${PROJECT_SOURCE_DIR}/extern/freetype")
    set(ENV{GLM_ROOT_DIR}   "${PROJECT_SOURCE_DIR}/extern/glm")
    set(ENV{OGG_ROOT}       "${PROJECT_SOURCE_DIR}/extern/ogg")
    set(ENV{SDL2DIR}        "${PROJECT_SOURCE_DIR}/extern/sdl2")
    set(ENV{SOUNDIO_ROOT}   "${PROJECT_SOURCE_DIR}/extern/soundio")
    set(ENV{VORBIS_ROOT}    "${PROJECT_SOURCE_DIR}/extern/vorbis")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Building for Linux.")
    set(PLATFORM PL_LINUX)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Building for Mac OS X.")
    set(PLATFORM PL_OSX)

    if(NOT DEFINED ENV{NO_APP})
        set(OSX_APP_BUNDLE 1)
    endif()

else()
    message(STATUS "Platform not detected !")
    set(PLATFORM PL_DEFAULT)
    # Error and abort here ?
endif()


####################################################################
#   Libraries
####################################################################

find_package(Freetype   REQUIRED)
find_package(GLM        REQUIRED)
find_package(OggVorbis  REQUIRED)
find_package(OpenGL     REQUIRED)
find_package(SDL2       REQUIRED)
find_package(SoundIO    REQUIRED)

if(NOT DEFINED(USE_LIB_PNGCPP))
    set(USE_LIB_PNGCPP 0)
endif()
if(USE_LIB_PNGCPP)
    find_package(PNG    REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/extern/png++)
    source_group("extern\\png++" FILES ${PNGPP_SRC})
endif()

add_library(GLAD STATIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c)
set_target_properties(GLAD PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}")

# These includes are defined a system headers so any warnings within
# don't get shown during compilation.
include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/extern/stb
    ${CMAKE_SOURCE_DIR}/extern/glad/include
    ${CMAKE_SOURCE_DIR}/extern/spdlog/include
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${OGG_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${SOUNDIO_INCLUDE_DIR}
    ${VORBIS_INCLUDE_DIR}
)

if(INC_PATH)
    string(REPLACE ":" ";" _INC_PATH ${INC_PATH})
    foreach(PATH ${_INC_PATH})
        include_directories(SYSTEM ${PATH})
    endforeach()
endif()
####################################################################
#   Source
####################################################################
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/smf.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/stringutils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/input.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.hpp
)
set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/smf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/stringutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.cpp
)

set(GAME_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/parser.hpp
)
set(GAME_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/parser.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/core/audio
    ${PROJECT_SOURCE_DIR}/src/engine
    ${PROJECT_SOURCE_DIR}/src/game
    ${PROJECT_SOURCE_DIR}/src
)

source_group("src\\core"  FILES ${CORE_SOURCE}  ${CORE_HEADERS})
source_group("src\\game"  FILES ${GAME_SOURCE}  ${GAME_HEADERS})

set(SOURCE
    ${CORE_HEADERS}  ${CORE_SOURCE}
    ${GAME_HEADERS}  ${GAME_SOURCE}
)

add_executable(openrhythm ${SOURCE})

set(BINARY_DATA_DIR ${CMAKE_BINARY_DIR}/data)
set(SOURCE_DATA_DIR ${CMAKE_SOURCE_DIR}/data)

add_custom_command(TARGET openrhythm PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                ${BINARY_DATA_DIR})
add_custom_command(TARGET openrhythm PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SOURCE_DATA_DIR} $<TARGET_FILE_DIR:openrhythm>/data/)


if(OSX_APP_BUNDLE)
    set_target_properties(openrhythm PROPERTIES MACOSX_BUNDLE TRUE)
endif()

target_link_libraries(openrhythm
    ${CMAKE_DL_LIBS}
      GLAD
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY}
    ${FREETYPE_LIBRARIES}
    ${SOUNDIO_LIBRARY}
    ${VORBISFILE_LIBRARY}
    ${VORBIS_LIBRARY}
    ${OGG_LIBRARY}
)


####################################################################
#   Documentation
####################################################################
find_package(Cldoc)

add_cldoc_documentation(
    FILES ${SOURCE}
    FLAGS
        -I${SDL2_INCLUDE_DIR}/.
        -I${SDL2Mixer_INCLUDE_DIRS}/.
        -I${LibVorbis_INCLUDE_DIRS}/.
        -I${PortAudio_INCLUDE_DIRS}/.
        -I${CMAKE_SOURCE_DIR}/extern/stb
        -I${CMAKE_SOURCE_DIR}/extern/glad/include
        -I${CMAKE_SOURCE_DIR}/extern/spdlog/include
        -I"${PROJECT_SOURCE_DIR}/src"
        -I"${PROJECT_SOURCE_DIR}/src/core"
        -I"${PROJECT_SOURCE_DIR}/src/core/audio"
        -I"${PROJECT_SOURCE_DIR}/src/engine"
        -I"${PROJECT_SOURCE_DIR}/src/game"
        -std=gnu++14
    OUTPUT "docs"
    LANGUAGE c++
)
