cmake_minimum_required(VERSION 3.0)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

project(musicgame)

set(PL_WINDOWS 1)
set(PL_LINUX 2)
set(PL_OSX 3)

set(PLATFORM 0)

if(NOT DEFINED(USE_LIB_PNGCPP))
    set(USE_LIB_PNGCPP 0)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Building for Windows.")
    set(ENV{SDL2DIR} "${PROJECT_SOURCE_DIR}/extern/sdl2")
    set(ENV{FREETYPE_DIR} "${PROJECT_SOURCE_DIR}/extern/freetype")
    set(ENV{GLM_ROOT_DIR} "${PROJECT_SOURCE_DIR}/extern/glm")
    set(PLATFORM PL_WINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Building for Linux.")
    set(PLATFORM PL_LINUX)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Building for Mac OS X.")
    set(PLATFORM PL_OSX)

endif()

find_package(SDL2 REQUIRED)
find_package(Portaudio  REQUIRED)
find_package(SoundIO    REQUIRED)
find_package(OggVorbis  REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/core/audio)
include_directories(${PROJECT_SOURCE_DIR}/src/engine)
include_directories(${PROJECT_SOURCE_DIR}/src/game)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codec.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/portaudio.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.hpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.hpp)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/portaudio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.cpp)

if(INC_PATH)
    string(REPLACE ":" ";" _INC_PATH ${INC_PATH})
    foreach(PATH ${_INC_PATH})
        include_directories(${PATH})
    endforeach()
endif()

source_group("src\\core" FILES ${CORE_SOURCE} ${CORE_HEADERS})

set(GAME_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/game/parser.cpp)
set(GAME_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/game/parser.hpp)

source_group("src\\game" FILES ${GAME_SOURCE} ${GAME_HEADERS})

include_directories()
set(GLAD ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c)
source_group("extern\\glad" FILES ${GLAD})


if(USE_LIB_PNGCPP)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/extern/png++)
    source_group("extern\\png++" FILES ${PNGPP_SRC})
endif()

# These includes are defined a system headers so any warnings within
# dont get shown during compilation.
include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/extern/stb
    ${CMAKE_SOURCE_DIR}/extern/glad/include
    ${CMAKE_SOURCE_DIR}/extern/spdlog/include
    ${SDL2_INCLUDE_DIR}
    ${SoundIO_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)

set(SOURCE ${CORE_HEADERS} ${CORE_SOURCE} ${GAME_SOURCE} ${GAME_HEADERS} ${GLAD})

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

target_link_libraries(musicgame
    ${CMAKE_DL_LIBS}
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY}
    ${FREETYPE_LIBRARIES}
    ${SOUNDIO_LIBRARY}
    ${VORBISFILE_LIBRARY}
    ${PORTAUDIO_LIBRARIES}

)
