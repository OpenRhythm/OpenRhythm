cmake_minimum_required(VERSION 3.3)

project (soundtouch-c)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

add_subdirectory(soundtouch)

add_library(SoundTouch OBJECT ${SOUNDTOUCH_SRC})
target_include_directories(SoundTouch PUBLIC ${SOUNDTOUCH_INCLUDE} PRIVATE ${SOUNDTOUCH_INCLUDE_PUB})
set_property(TARGET SoundTouch PROPERTY POSITION_INDEPENDENT_CODE ON)

include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${SOUNDTOUCH_INCLUDE_PUB})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(soundtouchc SHARED soundtouch-c.cpp $<TARGET_OBJECTS:SoundTouch>)

install(TARGETS soundtouchc DESTINATION ${CMAKE_INSTALL_PREFIX})

install(DIRECTORY ${CMAKE_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.h")
