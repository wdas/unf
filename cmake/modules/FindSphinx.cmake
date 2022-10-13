# Discover required Sphinx target.
#
# This module defines the following imported targets:
#     Sphinx::Build
#
# It also exposes the 'sphinx_add_docs' function which adds a target
# for generating documentation with Sphinx.
#
# Usage:
#     find_package(Sphinx)
#     find_package(Sphinx REQUIRED)
#     find_package(Sphinx 1.8.6 REQUIRED)
#
# Note:
#     The Sphinx_ROOT environment variable or CMake variable can be used to
#     prepend a custom search path.
#     (https://cmake.org/cmake/help/latest/policy/CMP0074.html)

include(FindPackageHandleStandardArgs)

find_program(SPHINX_EXECUTABLE NAMES sphinx-build)

if(SPHINX_EXECUTABLE)
    execute_process(
        COMMAND "${SPHINX_EXECUTABLE}" --version
        OUTPUT_VARIABLE _version
        ERROR_VARIABLE _version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_version MATCHES " ([0-9]+\\.[0-9]+\\.[0-9]+)$")
        set(SPHINX_VERSION "${CMAKE_MATCH_1}")
    endif()

    mark_as_advanced(_version)
endif()

mark_as_advanced(SPHINX_EXECUTABLE SPHINX_VERSION)

find_package_handle_standard_args(
    Sphinx
    REQUIRED_VARS
        SPHINX_EXECUTABLE
    VERSION_VAR
        SPHINX_VERSION
)

if (Sphinx_FOUND AND NOT TARGET Sphinx::Build)
    add_executable(Sphinx::Build IMPORTED GLOBAL)
    set_target_properties(Sphinx::Build PROPERTIES
        IMPORTED_LOCATION "${SPHINX_EXECUTABLE}"
    )

    function(sphinx_add_docs targetName)
        set(_comment "Generate documentation for ${targetName}")

        cmake_parse_arguments(PARSE_ARGV 1 _args "" "SOURCE;OUTPUT" "")

        add_custom_target(${targetName} VERBATIM
            COMMAND ${CMAKE_COMMAND} -E make_directory ${_args_OUTPUT}
            COMMAND Sphinx::Build -b html ${_args_SOURCE} ${_args_OUTPUT}
            COMMENT ${_comment}
        )
    endfunction()
endif()
