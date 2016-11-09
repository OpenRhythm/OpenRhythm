
conf = configuration_data()
# Surround the version in quotes to make it a C string
conf.set_quoted('PROJECT_NAME',  meson.project_name())
conf.set_quoted('VERSION'      , meson.project_version())
conf.set_quoted('VERSION_MAJOR', meson.project_version().split('.')[0])
conf.set_quoted('VERSION_MINOR', meson.project_version().split('.')[1])

configure_file(input  : 'config.hpp.in.meson',
               output : 'config.hpp',
               configuration : conf)


core_sources = [
    'core/audio/codecs/vorbis.cpp',
    'core/audio/output/soundio.cpp',
    'core/audio/streams/resample.cpp',
    'core/context.cpp',
    'core/events.cpp',
    'core/keycode.cpp',
    'core/mesh2d.cpp',
    'core/shader.cpp',
    'core/smf.cpp',
    'core/stringutils.cpp',
    'core/texture.cpp',
    'core/timing.cpp',
    'core/vfs.cpp',
    'core/window.cpp',
]

core_headers = [
    'core/audio/codecs/vorbis.hpp',
    'core/audio/output/chrono.hpp',
    'core/audio/output/soundio.hpp',
    'core/audio/stream.hpp',
    'core/audio/streams/resample.hpp',
    'core/configuration/parameter.hpp',
    'core/context.hpp',
    'core/events.hpp',
    'core/keycode.hpp',
    'core/mesh2d.hpp',
    'core/parseutils.hpp',
    'core/shader.hpp',
    'core/smf.hpp',
    'core/stringutils.hpp',
    'core/texture.hpp',
    'core/timing.hpp',
    'core/vfs.hpp',
    'core/window.hpp',
]

core_includes = include_directories(
    'core',
    'core/audio',
)

core = static_library('foo',
    core_sources,
    dependencies : [Glm, SDL2, SoundIO],
    include_directories : [syst_includes, core_includes],)

game_sources = [
    'game/configuration.cpp',
    'game/game.cpp',
    'game/parser.cpp',
]

game_headers = [
    'game/configuration.hpp',
    'game/game.hpp',
    'game/parser.hpp',
]


game_includes = include_directories(
    # 'engine',
    'game',
)

executable('openrhythm',
    'main.cpp', game_sources,
    include_directories : [syst_includes, core_includes, game_includes],
    dependencies : syst_libs,
         link_with : [core, GLAD])

# add_global_arguments('-DSOME_TOKEN=value', language : 'cpp')

#inc = include_directories('include')
