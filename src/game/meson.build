game_sources = [
    'configuration.cpp',
    'game.cpp',
    'parser.cpp',
]

game_headers = [
    'configuration.hpp',
    'game.hpp',
    'parser.hpp',
]

game_includes = include_directories('.')
game_dependencies = [syst_deps, core]

# Only useful to not compile twice this for tests
gamelib = static_library('game',
    game_sources,
    dependencies : game_dependencies,
    include_directories : [
        syst_includes,
        conf_includes,
        core_includes,
        game_includes,]
)


game = declare_dependency(
    include_directories : game_includes,
    link_with : gamelib,
    dependencies : syst_deps
)

executable('openrhythm',
    '../main.cpp',
    dependencies : game,
    include_directories : [
        syst_includes,
        core_includes,
        conf_includes,
        game_includes,],
    install : true,
)
