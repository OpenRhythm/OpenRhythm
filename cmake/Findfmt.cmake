# Variables defined:
#  FMT_FOUND
#  FMT_INCLUDE_DIR
#  FMT_LIBRARY
#
# Environment variables used:
#  FMT_ROOT

find_path(FMT_INCLUDE_DIR "fmt/format.h")
find_path(FMT_INCLUDE_DIR "fmt/format.h"
  HINTS $ENV{FMT_ROOT}/include)

find_library(FMT_LIBRARY NAMES fmt)
find_library(FMT_LIBRARY NAMES fmt
    HINTS $ENV{FMT_ROOT}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(fmt DEFAULT_MSG FMT_LIBRARY FMT_INCLUDE_DIR)

mark_as_advanced(FMT_LIBRARY FMT_INCLUDE_DIR)
