# Find Portaudio
# Defined or used in the script:
#  PORTAUDIO_FOUND - system has Portaudio
#  PORTAUDIO_INCLUDE_DIR - the Portaudio include directory
#  PORTAUDIO_LIBRARY - Link these to use Portaudio
# 
# Environment variables used:
#  PORTAUDIO_ROOT - Used to find library at a location.

find_path(PORTAUDIO_INCLUDE_DIR portaudio.h
	HINTS $ENV{PORTAUDIO_ROOT}/include)

find_library(PORTAUDIO_LIBRARY
	NAMES portaudio
	HINTS $ENV{PORTAUDIO_ROOT}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PORTAUDIO DEFAULT_MSG PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR)
mark_as_advanced(PORTAUDIO_LIBRARY PORTAUDIO_INCLUDE_DIR)

