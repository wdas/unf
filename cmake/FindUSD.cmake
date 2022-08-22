# Discover required Pixar USD targets.
#
# This module defines the following imported targets:
#     pxr::usd
#     pxr::sdf
#     pxr::tf
#     pxr::plug
#     pxr::arch
#     pxr::vt
#
# Usage:
#     find_package(USD)
#     find_package(USD REQUIRED)
#     find_package(USD 0.20.11 REQUIRED)
#
# Note:
#     The USD_ROOT environment variable or CMake variable can be used to
#     prepend a custom search path.
#     (https://cmake.org/cmake/help/latest/policy/CMP0074.html)
#
#     The PXR_LIB_PREFIX option can be used to indicate the expected
#     library prefix. By default, it will look for "libusd_" and "lib".
#     (https://github.com/PixarAnimationStudios/USD/blob/release/BUILDING.md)
#
#     We do not use pxrConfig.cmake to keep compatibility with USD included
#     within Presto.

include(FindPackageHandleStandardArgs)

if (NOT DEFINED PXR_LIB_PREFIX)
    set(PXR_LIB_PREFIX "usd_")
endif()

find_path(
    USD_INCLUDE_DIR
        pxr/pxr.h
    PATH_SUFFIXES
        include
)

set(USD_LIBRARIES usd sdf tf plug arch vt)

set(USD_DEPENDENCIES "Boost::boost;TBB::tbb")

if (BUILD_PYTHON_BINDINGS)
    set(USD_DEPENDENCIES  "${USD_DEPENDENCIES};Python::Python;Boost::python")
endif()

mark_as_advanced(USD_INCLUDE_DIR USD_LIBRARIES USD_DEPENDENCIES)

foreach(NAME IN LISTS USD_LIBRARIES)
    find_library(
        "${NAME}_LIBRARY"
        NAMES
            ${PXR_LIB_PREFIX}${NAME}
            ${NAME}
        PATH_SUFFIXES
            ${CMAKE_INSTALL_LIBDIR}
            lib
    )

    get_filename_component(${NAME}_LIBRARY "${${NAME}_LIBRARY}" REALPATH)

    mark_as_advanced("${NAME}_LIBRARY")
endforeach()

if(USD_INCLUDE_DIR AND EXISTS "${USD_INCLUDE_DIR}/pxr/pxr.h")
    file(READ "${USD_INCLUDE_DIR}/pxr/pxr.h" _pxr_header)
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
        USD_INCLUDE_DIR
        usd_LIBRARY
        sdf_LIBRARY
        tf_LIBRARY
        plug_LIBRARY
        arch_LIBRARY
        vt_LIBRARY
    VERSION_VAR
        USD_VERSION
)

if (USD_FOUND)
    foreach(NAME IN LISTS USD_LIBRARIES)
        if (NOT TARGET pxr::${NAME})
            add_library(pxr::${NAME} UNKNOWN IMPORTED)
            set_target_properties(pxr::${NAME} PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                INTERFACE_LINK_LIBRARIES "${USD_DEPENDENCIES}"
                IMPORTED_LOCATION "${${NAME}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${USD_INCLUDE_DIR}"
            )
        endif()
    endforeach()
endif()
