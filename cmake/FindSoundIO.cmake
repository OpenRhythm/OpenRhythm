# Variables defined:
#  SOUNDIO_FOUND
#  SOUNDIO_INCLUDE_DIR
#  SOUNDIO_LIBRARY
# 
# Environment variables used:
#  SOUNDIO_ROOT

find_path(SOUNDIO_INCLUDE_DIR soundio/soundio.h)
find_path(SOUNDIO_INCLUDE_DIR soundio/soundio.h
	HINTS $ENV{SOUNDIO_ROOT}/include)

find_library(SOUNDIO_LIBRARY NAMES soundio)
find_library(SOUNDIO_LIBRARY NAMES soundio
    HINTS $ENV{SOUNDIO_ROOT}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SOUNDIO DEFAULT_MSG SOUNDIO_LIBRARY SOUNDIO_INCLUDE_DIR)

mark_as_advanced(SOUNDIO_LIBRARY SOUNDIO_INCLUDE_DIR)
