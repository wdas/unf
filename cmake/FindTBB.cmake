# Discover TBB package.
#
# Variable defined by this module:
#     TBB_INCLUDE_DIR
#     TBB_LIBRARY_DIR
#     TBB_LIBRARY
#     TBB_MAJOR_VERSION
#     TBB_MINOR_VERSION
#     TBB_PATCH_VERSION
#     TBB_VERSION
#
# Usage:
#     find_package(TBB)
#     find_package(TBB REQUIRED)
# 
# Note:
#     The TBB_LOCATION environment variable can be used as a hint.
#

find_path(
    TBB_INCLUDE_DIR
    NAMES
        tbb/tbb.h
    HINTS
        ${TBB_LOCATION}
        $ENV{TBB_LOCATION}
    PATH_SUFFIXES
        include
    DOC "TBB Include directory"
)

find_library(
    TBB_LIBRARY
    NAMES
        tbb
    HINTS
        ${TBB_LOCATION}
        $ENV{TBB_LOCATION}
    PATH_SUFFIXES
        ${CMAKE_INSTALL_LIBDIR}
        lib
    DOC
        "Main TBB library"
)

get_filename_component(TBB_LIBRARY_DIR ${TBB_LIBRARY} DIRECTORY)

if(TBB_INCLUDE_DIR AND EXISTS "${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h")
    file(READ "${TBB_INCLUDE_DIR}/tbb/tbb_stddef.h" _tbb_header)
    foreach(_element MAJOR MINOR)
        string(
            REGEX REPLACE ".*#define TBB_VERSION_${_element} ([0-9]+).*" "\\1"
            TBB_${_element}_VERSION "${_tbb_header}"
        )
    endforeach()
    set(TBB_VERSION ${TBB_MAJOR_VERSION}.${TBB_MINOR_VERSION})
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    TBB
    REQUIRED_VARS
        TBB_INCLUDE_DIR
        TBB_LIBRARY_DIR
        TBB_LIBRARY
    VERSION_VAR
        TBB_VERSION
)
