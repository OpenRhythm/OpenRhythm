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
if(MSVC)
    add_definitions("/W4")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-Wall")
    # Clean unused sections on link
    add_definitions("-ffunction-sections -fdata-sections -Wl,--gc-sections")
    # Colored output
    add_definitions("-fdiagnostics-color")
endif()

####################################################################
#   Platform detection and rules
####################################################################
set(PL_DEFAULT  0)
set(PL_WINDOWS  1)
set(PL_LINUX    2)
set(PL_OSX      3)

set(OSX_APP_BUNDLE 0)

## This is needed when cross-compiling
list(APPEND CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/extern")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Building for Windows.")
    set(PLATFORM PL_WINDOWS)

    set(ENV{FREETYPE_DIR}    "${PROJECT_SOURCE_DIR}/extern/freetype")
    set(ENV{GLM_ROOT_DIR}    "${PROJECT_SOURCE_DIR}/extern/glm")
    set(ENV{OGG_ROOT}        "${PROJECT_SOURCE_DIR}/extern/ogg")
    set(ENV{SAMPLERATE_ROOT} "${PROJECT_SOURCE_DIR}/extern/samplerate")
    set(ENV{SDL2DIR}         "${PROJECT_SOURCE_DIR}/extern/sdl2")
    set(ENV{SOUNDIO_ROOT}    "${PROJECT_SOURCE_DIR}/extern/soundio")
    set(ENV{TCLAP_ROOT}      "${PROJECT_SOURCE_DIR}/extern/tclap")    # TODO add the TCLAP files
    set(ENV{VORBIS_ROOT}     "${PROJECT_SOURCE_DIR}/extern/vorbis")
    set(ENV{YAMLCPP_ROOT}    "${PROJECT_SOURCE_DIR}/extern/yaml-cpp")
    set(ENV{FMT_ROOT}    "${PROJECT_SOURCE_DIR}/extern/fmt")
    set(Gettext_BINARIES     "${PROJECT_SOURCE_DIR}/extern/gettext")


    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
    endif()

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
    message(STATUS "Platform not detected!")
    set(PLATFORM PL_DEFAULT)
    # Error and abort here ?
endif()


####################################################################
#   Libraries
####################################################################
# The *_ROOT variables specify the location of a local clone
# to be used if a system-wide library is not found

find_package(Freetype   REQUIRED)
find_package(Gettext    )
find_package(GLM        REQUIRED)
find_package(OggVorbis  REQUIRED)
find_package(OpenGL     REQUIRED)
find_package(SampleRate REQUIRED)
find_package(SDL2       REQUIRED)
find_package(SoundIO    REQUIRED)
find_package(TCLAP      REQUIRED)
find_package(YamlCpp    REQUIRED)
find_package(fmt        REQUIRED)

add_library(GLAD STATIC ${CMAKE_CURRENT_SOURCE_DIR}/extern/glad/src/glad.c)
set_target_properties(GLAD PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}")

set(LIBRARIES
      GLAD
    ${CMAKE_DL_LIBS}
    ${FREETYPE_LIBRARIES}
    ${OGG_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${SAMPLERATE_LIBRARY}
    ${SDL2_LIBRARY}
    ${SOUNDIO_LIBRARY}
    ${VORBIS_LIBRARY}
    ${VORBISFILE_LIBRARY}
    ${YAMLCPP_LIBRARY}
    ${FMT_LIBRARY}
)


# These includes are defined a system headers so any warnings within
# don't get shown during compilation.
include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/extern/glad/include
    ${CMAKE_SOURCE_DIR}/extern/readerwriterqueue
    ${CMAKE_SOURCE_DIR}/extern/spdlog/include
    ${CMAKE_SOURCE_DIR}/extern/stb
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${OGG_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${SAMPLERATE_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${SOUNDIO_INCLUDE_DIR}
    ${TCLAP_INCLUDE_DIR}
    ${VORBIS_INCLUDE_DIR}
    ${YAMLCPP_INCLUDE_DIR}
    ${FMT_INCLUDE_DIR}
)

if(INC_PATH)
    string(REPLACE ":" ";" _INC_PATH ${INC_PATH})
    foreach(PATH ${_INC_PATH})
        include_directories(SYSTEM ${PATH})
    endforeach()
endif()

if(GETTEXT_FOUND)
    set(TRANSLATE_ENABLED "true")
    message("Translation enabled")
else()
    set(TRANSLATE_ENABLED "false")
    message("Translation disabled")
endif()


####################################################################
#   Source
####################################################################
configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
  "${PROJECT_SOURCE_DIR}/src/config.hpp"
)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/stream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/streams/resample.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/configuration/parameter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh2d.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/parseutils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/smf.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/stringutils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/texture.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.hpp
)
set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/codecs/vorbis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/output/soundio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio/streams/resample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/smf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/stringutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/vfs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.cpp
)

set(GAME_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/configuration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/song.hpp
)
set(GAME_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/song.cpp
)

set(ALL_SOURCE
    ${CORE_SOURCE} ${CORE_HEADERS} ${GAME_SOURCE} ${GAME_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/audiotests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/general_tests.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/core/audio
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/engine
    ${PROJECT_SOURCE_DIR}/src/game
    ${PROJECT_SOURCE_DIR}/src
)

# To create tabs in VisualStudio
source_group("src\\core"  FILES ${CORE_SOURCE}  ${CORE_HEADERS})
source_group("src\\game"  FILES ${GAME_SOURCE}  ${GAME_HEADERS})

add_library(ORCore-obj OBJECT ${CORE_SOURCE})

add_executable(       openrhythm $<TARGET_OBJECTS:ORCore-obj>
                                 ${GAME_SOURCE}
                                 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(openrhythm ${LIBRARIES})
install(TARGETS       openrhythm DESTINATION bin)

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

####################################################################
#   Localization
####################################################################
if(GETTEXT_FOUND)
    set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")
    include(GettextTranslate)
    GettextTranslateTargets()

    add_subdirectory(po)
endif()

####################################################################
#   Test binaries
####################################################################
# TODO - Make the tests not require the game source because of recompilation.
add_executable(       audiotests $<TARGET_OBJECTS:ORCore-obj>
                                 ${GAME_SOURCE}
                                 ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/audiotests.cpp)
target_link_libraries(audiotests ${LIBRARIES})
install(TARGETS       audiotests DESTINATION bin)

add_executable(       general_tests $<TARGET_OBJECTS:ORCore-obj>
                                    ${GAME_SOURCE}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/general_tests.cpp)
target_link_libraries(general_tests ${LIBRARIES})
install(TARGETS       general_tests DESTINATION bin)


####################################################################
#   Documentation
####################################################################
find_package(Cldoc)

if(Cldoc_FOUND)
    # Adds "doc" and "doc/fast" make targets
    add_cldoc_documentation(
        FILES ${ALL_SOURCE}
        FLAGS
            -I${SDL2_INCLUDE_DIR}/.
            -I${SDL2Mixer_INCLUDE_DIRS}/.
            -I${LibVorbis_INCLUDE_DIRS}/.
            -I${PortAudio_INCLUDE_DIRS}/.
            -I${CMAKE_SOURCE_DIR}/extern/stb
            -I${CMAKE_SOURCE_DIR}/extern/glad/include
            -I${CMAKE_SOURCE_DIR}/extern/spdlog/include
            -I"${CMAKE_CURRENT_SOURCE_DIR}/src"
            -I"${CMAKE_CURRENT_SOURCE_DIR}/src/core"
            -I"${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio"
            -I"${CMAKE_CURRENT_SOURCE_DIR}/src/engine"
            -I"${CMAKE_CURRENT_SOURCE_DIR}/src/game"
            -std=gnu++14
        OUTPUT "docs"
        LANGUAGE c++
    )
endif()
