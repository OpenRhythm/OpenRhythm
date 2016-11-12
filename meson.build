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

GLAD_includes = include_directories('extern/glad/include', is_system : true)
GLADlib = static_library('GLAD',
    'extern/glad/src/glad.c',
    include_directories : GLAD_includes
)
GLAD = declare_dependency(
    include_directories : GLAD_includes,
    link_with : GLADlib,
)

syst_includes = include_directories(
    'extern/spdlog/include',
    'extern/stb',
    is_system : true,
)

syst_deps = [
    meson.get_compiler('cpp').find_library('dl'),
    freetype,
    # Gettext
    Glm,
    GLAD,
    vorbisFile,
    OpenGL,
    SampleRate,
    SDL2,
    SoundIO,
    YAMLCpp,
]


subdir('src')
subdir('po')



install_data(
    sources : [
        'data/',
    ],
    install_dir : 'share/openrhythm')

# if(OSX_APP_BUNDLE)
#     set_target_properties(openrhythm PROPERTIES MACOSX_BUNDLE TRUE)
# endif()



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
