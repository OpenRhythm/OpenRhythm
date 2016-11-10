
conf = configuration_data()
# Surround the version in quotes to make it a C string
conf.set_quoted('PROJECT_NAME',  meson.project_name())
conf.set_quoted('VERSION'      , meson.project_version())
conf.set_quoted('VERSION_MAJOR', meson.project_version().split('.')[0])
conf.set_quoted('VERSION_MINOR', meson.project_version().split('.')[1])

configure_file(input  : 'config.hpp.in.meson',
               output : 'config.hpp',
               configuration : conf)

conf_includes = include_directories('.')

subdir('core')
subdir('game')
subdir('tests')



# add_global_arguments('-DSOME_TOKEN=value', language : 'cpp')

