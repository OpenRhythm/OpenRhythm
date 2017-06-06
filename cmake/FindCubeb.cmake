# Variables defined:
#  CUBEB_FOUND
#  CUBEB_INCLUDE_DIR
#  CUBEB_LIBRARY
# 
# Environment variables used:
#  CUBEB_ROOT

find_path(CUBEB_INCLUDE_DIR cubeb/cubeb.h)
find_path(CUBEB_INCLUDE_DIR cubeb/cubeb.h
	HINTS $ENV{CUBEB_ROOT}/include)

find_library(CUBEB_LIBRARY NAMES cubeb)
find_library(CUBEB_LIBRARY NAMES cubeb
    HINTS $ENV{CUBEB_ROOT})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CUBEB DEFAULT_MSG CUBEB_LIBRARY CUBEB_INCLUDE_DIR)

mark_as_advanced(CUBEB_LIBRARY CUBEB_INCLUDE_DIR)
