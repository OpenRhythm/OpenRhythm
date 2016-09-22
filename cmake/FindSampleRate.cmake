# Base Io build system
# Written by Jeremy Tregunna <jeremy.tregunna@me.com>
#
# Find libsamplerate.

find_path(SAMPLERATE_INCLUDE_DIR samplerate.h
    HINTS $ENV{SOUNDIO_ROOT}/include)

set(SAMPLERATE_NAMES ${SAMPLERATE_NAMES} samplerate libsamplerate)

find_library(SAMPLERATE_LIBRARY NAMES ${SAMPLERATE_NAMES}
    HINTS $ENV{SOUNDIO_ROOT}/lib
    PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SampleRate DEFAULT_MSG
    SAMPLERATE_LIBRARY
    SAMPLERATE_INCLUDE_DIR)

mark_as_advanced(SAMPLERATE_LIBRARY SAMPLERATE_INCLUDE_DIR)
