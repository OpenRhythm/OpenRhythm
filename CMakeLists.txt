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
    set(PLATFORM PL_WINDOWS)

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Building for Linux.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -lGL")
    set(PLATFORM PL_LINUX)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "Building for Mac OS X.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
    set(ADDITIONAL_LIBS "-framework OpenGL")
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/keycode.hpp)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/events.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/timing.cpp)

source_group("src\\core" FILES ${CORE_SOURCE} ${CORE_HEADERS})


set(GAME_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/game/main.cpp)

source_group("src\\game" FILES ${GAME_SOURCE})


set(GL ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_core33.h
       ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl_core33.c
       ${CMAKE_CURRENT_SOURCE_DIR}/src/gl/gl.hpp)

source_group("src\\gl" FILES ${GL})


set( SOURCE ${CORE_HEADERS} ${CORE_SOURCE} ${GAME_SOURCE} ${GL})

add_executable(musicgame ${SOURCE})

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_target_properties(musicgame PROPERTIES MACOSX_BUNDLE TRUE)
endif()

target_link_libraries(musicgame ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${FREETYPE_LIBRARIES} ${ADDITIONAL_LIBS})
