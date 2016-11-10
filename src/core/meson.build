core_sources = [
    'audio/codecs/vorbis.cpp',
    'audio/output/soundio.cpp',
    'audio/streams/resample.cpp',
    'context.cpp',
    'events.cpp',
    'keycode.cpp',
    'mesh2d.cpp',
    'shader.cpp',
    'smf.cpp',
    'stringutils.cpp',
    'texture.cpp',
    'timing.cpp',
    'vfs.cpp',
    'window.cpp',
]

core_headers = [
    'audio/codecs/vorbis.hpp',
    'audio/output/chrono.hpp',
    'audio/output/soundio.hpp',
    'audio/stream.hpp',
    'audio/streams/resample.hpp',
    'configuration/parameter.hpp',
    'context.hpp',
    'events.hpp',
    'keycode.hpp',
    'mesh2d.hpp',
    'parseutils.hpp',
    'shader.hpp',
    'smf.hpp',
    'stringutils.hpp',
    'texture.hpp',
    'timing.hpp',
    'vfs.hpp',
    'window.hpp',
]

core_includes = include_directories('.', 'audio')
core_dependencies = [syst_deps]

corelib = static_library('core',
    core_sources,
    dependencies : core_dependencies,
    include_directories : [
        syst_includes,
        conf_includes,
        core_includes]
)


core = declare_dependency(
    include_directories : core_includes,
    link_with : corelib,
)
