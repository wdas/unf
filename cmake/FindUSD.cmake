# Discover Pixar USD package.
#
# Variable defined by this module:
#     USD_INCLUDE_DIR
#     USD_LIBRARY_DIR
#     USD_LIBRARY
#     USD_MAJOR_VERSION
#     USD_MINOR_VERSION
#     USD_PATCH_VERSION
#     USD_VERSION
#
# Usage:
#     find_package(USD)
#     find_package(USD REQUIRED)
# 
# Note:
#     The PXR_USD_LOCATION environment variable can be used as a hint.
#
#     The PXR_USD_PREFIX environment variable can be used to indicate the 
#     expected library prefix. By default, it will look for "libusd_" and "lib".
#
#     We do not use pxrConfig.cmake to keep compatibility with USD included
#     within Presto.
#

if (NOT DEFINED PXR_USD_PREFIX)
    set(PXR_USD_PREFIX "usd_")
endif()

find_path(
    USD_INCLUDE_DIR
    NAMES
        pxr/pxr.h
    HINTS
        ${PXR_USD_LOCATION}
        $ENV{PXR_USD_LOCATION}
    PATH_SUFFIXES
        include
    DOC "USD Include directory"
)

set(USD_LIBRARIES, "")

foreach(component usd sdf tf arch) 
    find_library(
        "${component}_LIBRARY"
        NAMES
            ${PXR_USD_PREFIX}${component}
            ${component}
        HINTS
            ${PXR_USD_LOCATION}
            $ENV{PXR_USD_LOCATION}
        PATH_SUFFIXES
            ${CMAKE_INSTALL_LIBDIR}
            lib
        DOC
            "${component} library"
    )

    list(APPEND USD_LIBRARIES "${${component}_LIBRARY}")

endforeach()

get_filename_component(USD_LIBRARY_DIR ${usd_LIBRARY} DIRECTORY)

if(USD_INCLUDE_DIR AND EXISTS "${USD_INCLUDE_DIR}/pxr/pxr.h")
    file(READ "${USD_INCLUDE_DIR}/pxr/pxr.h" _pxr_header)
    foreach(_element MAJOR MINOR PATCH)
        string(
            REGEX REPLACE ".*#define PXR_${_element}_VERSION ([0-9]+).*" "\\1"
            USD_${_element}_VERSION "${_pxr_header}"
        )
    endforeach()
    set(
        USD_VERSION 
        ${USD_MAJOR_VERSION}.${USD_MINOR_VERSION}.${USD_PATCH_VERSION}
    )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    USD
    REQUIRED_VARS
        USD_INCLUDE_DIR
        USD_LIBRARY_DIR
        USD_LIBRARIES
    VERSION_VAR
        USD_VERSION
)
