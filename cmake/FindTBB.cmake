# Discover required TBB target.
#
# Only headers are necessary to build this repository, so libraries
# are ignored.
#
# This module defines the following imported targets:
#     TBB::tbb
#
# Usage:
#     find_package(TBB)
#     find_package(TBB REQUIRED)
#     find_package(TBB 2020.3 REQUIRED)
#
# Note:
#     The TBB_LOCATION environment variable can be used as a hint.

include(FindPackageHandleStandardArgs)

find_path(
    TBB_INCLUDE_DIR
        tbb/tbb.h
    HINTS
        ${TBB_LOCATION}
        $ENV{TBB_LOCATION}
    PATH_SUFFIXES
        include
)

mark_as_advanced(TBB_INCLUDE_DIR)

if(TBB_INCLUDE_DIR AND EXISTS "${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h")
    file(READ "${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h" _tbb_header)
    foreach(label MAJOR MINOR)
        string(
            REGEX REPLACE ".*#define TBB_VERSION_${label} ([0-9]+).*" "\\1"
            _tbb_${label} "${_tbb_header}"
        )
    endforeach()

    set(TBB_VERSION ${_tbb_MAJOR}.${_tbb_MINOR})

    mark_as_advanced(
        _tbb_MAJOR
        _tbb_MINOR
        TBB_VERSION
    )
endif()

find_package_handle_standard_args(
    TBB
    REQUIRED_VARS
        TBB_INCLUDE_DIR
    VERSION_VAR
        TBB_VERSION
)

if (TBB_FOUND AND NOT TARGET TBB::tbb)
    add_library(TBB::tbb INTERFACE IMPORTED)
    target_include_directories(TBB::tbb INTERFACE "${TBB_INCLUDE_DIR}")
endif()
