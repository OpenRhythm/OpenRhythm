####################################################################
#   Test binaries
####################################################################

executable(
    'audiotests',
    'audiotests.cpp',
    dependencies : core,
    include_directories : [
        syst_includes,
        conf_includes,
        game_includes,]
)

executable(
    'general_tests',
    'general_tests.cpp',
    dependencies : [core, game],
    include_directories : [
        syst_includes,
        conf_includes,
        game_includes,]
)
