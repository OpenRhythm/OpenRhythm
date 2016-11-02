# Variables defined:
#  TCLAP_FOUND
#  TCLAP_INCLUDE_DIR
#
# Environment variables used:
#  TCLAP_ROOT

find_path(TCLAP_INCLUDE_DIR tclap/Arg.h)
find_path(TCLAP_INCLUDE_DIR tclap/Arg.h
  HINTS $ENV{TCLAP_ROOT}/)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TCLAP DEFAULT_MSG TCLAP_INCLUDE_DIR)

mark_as_advanced(TCLAP_INCLUDE_DIR)
