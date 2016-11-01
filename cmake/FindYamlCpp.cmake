# Variables defined:
#  YAMLCPP_FOUND
#  YAMLCPP_INCLUDE_DIR
#  YAMLCPP_LIBRARY
# 
# Environment variables used:
#  YAMLCPP_ROOT

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h
  HINTS $ENV{YAMLCPP_ROOT}/include)

find_library(YAMLCPP_LIBRARY NAMES yaml-cpp)
find_library(YAMLCPP_LIBRARY NAMES yaml-cpp
    HINTS $ENV{YAMLCPP_ROOT}/lib)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCPP DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)

mark_as_advanced(YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)
