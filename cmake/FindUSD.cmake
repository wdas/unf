# Discover required Pixar USD targets.
#
# This module defines the following imported targets:
#     pxr::usd
#     pxr::sdf
#     pxr::tf
#     pxr::arch
#
# Usage:
#     find_package(USD)
#     find_package(USD REQUIRED)
#     find_package(USD 0.20.11 REQUIRED)
# 
# Note:
#     The PXR_USD_LOCATION environment variable can be used as a hint.
#
#     The PXR_USD_PREFIX environment variable can be used to indicate the 
#     expected library prefix. By default, it will look for "libusd_" and "lib".
#
#     We do not use pxrConfig.cmake to keep compatibility with USD included
#     within Presto.

include(FindPackageHandleStandardArgs)

if (NOT DEFINED PXR_USD_PREFIX)
    set(PXR_USD_PREFIX "usd_")
endif()

find_path(
    PXR_INCLUDE_DIR 
        pxr/pxr.h
    HINTS
        ${PXR_USD_LOCATION}
        $ENV{PXR_USD_LOCATION}
    PATH_SUFFIXES
        include
)

set(PXR_LIBRARIES usd sdf tf arch)

mark_as_advanced(PXR_INCLUDE_DIR PXR_LIBRARIES)

foreach(NAME IN LISTS PXR_LIBRARIES)
    find_library(
        "${NAME}_LIBRARY"
        NAMES
            ${PXR_USD_PREFIX}${NAME}
            ${NAME}
        HINTS
            ${PXR_USD_LOCATION}
            $ENV{PXR_USD_LOCATION}
        PATH_SUFFIXES
            ${CMAKE_INSTALL_LIBDIR}
            lib
    )

    get_filename_component(${NAME}_LIBRARY "${${NAME}_LIBRARY}" REALPATH)

    mark_as_advanced("${NAME}_LIBRARY")
endforeach()

if(PXR_INCLUDE_DIR AND EXISTS "${PXR_INCLUDE_DIR}/pxr/pxr.h")
    file(READ "${PXR_INCLUDE_DIR}/pxr/pxr.h" _pxr_header)
    foreach(label MAJOR MINOR PATCH)
        string(
            REGEX REPLACE ".*#define PXR_${label}_VERSION ([0-9]+).*" "\\1"
            _pxr_${label} "${_pxr_header}"
        )
    endforeach()

    set(USD_VERSION ${_pxr_MAJOR}.${_pxr_MINOR}.${_pxr_PATCH})

    mark_as_advanced(
        _pxr_MAJOR
        _pxr_MINOR
        _pxr_PATCH
        USD_VERSION
    )
endif()

find_package_handle_standard_args(
    USD
    REQUIRED_VARS
        PXR_INCLUDE_DIR
        usd_LIBRARY
        sdf_LIBRARY
        tf_LIBRARY
        arch_LIBRARY
    VERSION_VAR
        USD_VERSION
)

if (USD_FOUND)
    foreach(NAME IN LISTS PXR_LIBRARIES)
        if (NOT TARGET pxr::${NAME})
            add_library(pxr::${NAME} UNKNOWN IMPORTED)
            set_target_properties(pxr::${NAME} PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                IMPORTED_LOCATION "${${NAME}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PXR_INCLUDE_DIR}"
            )
        endif()
    endforeach()
endif()
