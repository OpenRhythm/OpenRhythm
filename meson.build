####################################################################
#   Project configuration
####################################################################
project('OpenRhythm', 'c', 'cpp',
    version : '0.1',
    license : 'GPLv3',
    default_options : [
        'c_std=c11', 'cpp_std=c++14',
        ]
    )

AppleAppName = 'openrhythm.app'

####################################################################
#   C++ / C flags
####################################################################
compiler_id = meson.get_compiler('cpp').get_id()

if (compiler_id == 'msvc')
    add_global_arguments(
        '-/W4',
        '/wd4244',
        language : 'cpp')
endif

if (compiler_id == 'gcc'
 or compiler_id == 'clang')
    add_global_arguments(
        '-Wall',
        # Clean unused sections on link
        '-ffunction-sections',
        '-fdata-sections',
        '-Wl,--gc-sections',
        # Colored output
        '-fdiagnostics-color',
        language : 'cpp')
endif

####################################################################
#   Platform detection and rules
####################################################################
# set(PL_DEFAULT  0)
# set(PL_WINDOWS  1)
# set(PL_LINUX    2)
# set(PL_OSX      3)

# set(OSX_APP_BUNDLE 0)

if   (host_machine.system() == 'windows')
    message('Building for Windows.')
    Freetype_dir = 'extern/freetype'
    GLM_dir      = 'extern/glm'
    Ogg_dir      = 'extern/ogg'
    SDL2_dir     = 'extern/sdl2'
    Vorbis_dir   = 'extern/vorbis'

elif (host_machine.system() == 'linux')
    message('Building for Linux.')

elif (host_machine.system() == 'osx')
# Missing doc, what for MacOS ?
    message('Building for Mac OS X.')
    if not NO_APP
        OSX_APP_BUNDLE = 1
    endif

else
    message('Platform not detected !')
    exit()

endif




####################################################################
#   Libraries
####################################################################

freetype    = dependency('freetype2')
# Gettext
Glm         = dependency('glm')
vorbisFile  = dependency('vorbisfile')
OpenGL      = dependency('gl')
SampleRate  = dependency('samplerate')
SDL2        = dependency('sdl2')
SoundIO     = meson.get_compiler('cpp').find_library('soundio')
YAMLCpp     = dependency('yaml-cpp')

GLAD_includes = include_directories('extern/glad/include')
GLAD = static_library('glad',
    'extern/glad/src/glad.c',
    dependencies : [],
    include_directories : [GLAD_includes],)


syst_includes = include_directories(
    'extern/glad/include',
    'extern/spdlog/include',
    'extern/stb',
    is_system : true,
)

syst_libs = [
    meson.get_compiler('cpp').find_library('dl'),
    freetype,
    # Gettext
    Glm,
    vorbisFile,
    OpenGL,
    SampleRate,
    SDL2,
    SoundIO,
    YAMLCpp,
]


subdir('src')



####################################################################
#   Localization
####################################################################
# set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")
# include(GettextTranslate)
# GettextTranslateTargets(ALL)

# add_subdirectory(po)

# install(TARGETS       openrhythm DESTINATION bin)

# set(BINARY_DATA_DIR ${CMAKE_BINARY_DIR}/data)
# set(SOURCE_DATA_DIR ${CMAKE_SOURCE_DIR}/data)

# add_custom_command(TARGET openrhythm PRE_BUILD
#                 COMMAND ${CMAKE_COMMAND} -E make_directory
#                 ${BINARY_DATA_DIR})
# add_custom_command(TARGET openrhythm PRE_BUILD
#                 COMMAND ${CMAKE_COMMAND} -E copy_directory
#                 ${SOURCE_DATA_DIR} $<TARGET_FILE_DIR:openrhythm>/data/)


# if(OSX_APP_BUNDLE)
#     set_target_properties(openrhythm PROPERTIES MACOSX_BUNDLE TRUE)
# endif()


# ####################################################################
# #   Test binaries
# ####################################################################
# add_executable(       audiotests $<TARGET_OBJECTS:ORCore-obj> ${PROJECT_SOURCE_DIR}/src/tests/audiotests.cpp)
# target_link_libraries(audiotests ${LIBRARIES})
# install(TARGETS       audiotests DESTINATION bin)

# add_executable(       general_tests $<TARGET_OBJECTS:ORCore-obj> ${PROJECT_SOURCE_DIR}/src/tests/general_tests.cpp)
# target_link_libraries(general_tests ${LIBRARIES})
# install(TARGETS       general_tests DESTINATION bin)


####################################################################
#   Documentation
####################################################################
# find_package(Cldoc)

# if(Cldoc_FOUND)
#     # Adds "doc" and "doc/fast" make targets
#     add_cldoc_documentation(
#         FILES ${CORE_SOURCE} ${CORE_HEADERS} ${GAME_SOURCE} ${GAME_HEADERS}
#         FLAGS
#             -I${SDL2_INCLUDE_DIR}/.
#             -I${SDL2Mixer_INCLUDE_DIRS}/.
#             -I${LibVorbis_INCLUDE_DIRS}/.
#             -I${PortAudio_INCLUDE_DIRS}/.
#             -I${CMAKE_SOURCE_DIR}/extern/stb
#             -I${CMAKE_SOURCE_DIR}/extern/glad/include
#             -I${CMAKE_SOURCE_DIR}/extern/spdlog/include
#             -I"${PROJECT_SOURCE_DIR}/src"
#             -I"${PROJECT_SOURCE_DIR}/src/core"
#             -I"${PROJECT_SOURCE_DIR}/src/core/audio"
#             -I"${PROJECT_SOURCE_DIR}/src/engine"
#             -I"${PROJECT_SOURCE_DIR}/src/game"
#             -std=gnu++14
#         OUTPUT "docs"
#         LANGUAGE c++
#     )
# endif()
