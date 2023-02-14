# Discover required Pytest target.
#
# This module defines the following imported targets:
#     Pytest::Pytest
#
# It also exposes the 'pytest_discover_tests' function which adds ctest
# for each pytest tests. The "BUNDLE_PYTHON_TESTS" environment variable
# can be used to run all discovered tests all together.
#
# Usage:
#     find_package(Pytest)
#     find_package(Pytest REQUIRED)
#     find_package(Pytest 4.6.11 REQUIRED)
#
# Note:
#     The Pytest_ROOT environment variable or CMake variable can be used to
#     prepend a custom search path.
#     (https://cmake.org/cmake/help/latest/policy/CMP0074.html)

include(FindPackageHandleStandardArgs)

find_program(PYTEST_EXECUTABLE NAMES pytest)
mark_as_advanced(PYTEST_EXECUTABLE)

if(PYTEST_EXECUTABLE)
    execute_process(
        COMMAND "${PYTEST_EXECUTABLE}" --version
        OUTPUT_VARIABLE _version
        ERROR_VARIABLE _version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_version MATCHES "pytest (version )?([0-9]+\\.[0-9]+\\.[0-9]+)")
        set(PYTEST_VERSION "${CMAKE_MATCH_2}")
    endif()
endif()

find_package_handle_standard_args(
    Pytest
    REQUIRED_VARS
        PYTEST_EXECUTABLE
    VERSION_VAR
        PYTEST_VERSION
    HANDLE_COMPONENTS
)

if (Pytest_FOUND AND NOT TARGET Pytest::Pytest)
    add_executable(Pytest::Pytest IMPORTED)
    set_target_properties(Pytest::Pytest PROPERTIES
        IMPORTED_LOCATION "${PYTEST_EXECUTABLE}"
    )

    function(pytest_discover_tests NAME)
        cmake_parse_arguments(
            PARSE_ARGV 1 "" ""
            "WORKING_DIRECTORY;TRIM_FROM_NAME;BUNDLE_TESTS"
            "LIBRARY_PATH_PREPEND;PYTHON_PATH_PREPEND;DEPENDS"
        )

        set(libpath $ENV{LD_LIBRARY_PATH})
        set(pythonpath $ENV{PYTHONPATH})

        if (_LIBRARY_PATH_PREPEND)
            list(REVERSE "${_LIBRARY_PATH_PREPEND}")
            foreach (_path ${_LIBRARY_PATH_PREPEND})
                set(libpath "${_path}:${libpath}")
            endforeach()
        endif()

        if (_PYTHON_PATH_PREPEND)
            list(REVERSE "${_PYTHON_PATH_PREPEND}")
            foreach (_path ${_PYTHON_PATH_PREPEND})
                set(pythonpath "${_path}:${pythonpath}")
            endforeach()
        endif()

        if (NOT _WORKING_DIRECTORY)
            set(_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif()

        # Override option by environment variable if available.
        if (DEFINED ENV{BUNDLE_PYTHON_TESTS})
            set(_BUNDLE_TESTS $ENV{BUNDLE_PYTHON_TESTS})
        endif()

        set(_tests_file "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_tests.cmake")

        add_custom_target(
            ${NAME} ALL VERBATIM
            BYPRODUCTS "${_tests_file}"
            DEPENDS ${_DEPENDS}
            COMMAND ${CMAKE_COMMAND}
                -D "PYTEST_EXECUTABLE=${PYTEST_EXECUTABLE}"
                -D "TEST_GROUP_NAME=${NAME}"
                -D "BUNDLE_TESTS=${_BUNDLE_TESTS}"
                -D "LIBRARY_PATH=${libpath}"
                -D "PYTHON_PATH=${pythonpath}"
                -D "TRIM_FROM_NAME=${_TRIM_FROM_NAME}"
                -D "WORKING_DIRECTORY=${_WORKING_DIRECTORY}"
                -D "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
                -D "CTEST_FILE=${_tests_file}"
                -P "${_PYTEST_DISCOVER_TESTS_SCRIPT}"
        )

        # Add discovered tests to directory TEST_INCLUDE_FILES
        set_property(DIRECTORY
            APPEND PROPERTY TEST_INCLUDE_FILES "${_tests_file}"
        )
    endfunction()

    set(_PYTEST_DISCOVER_TESTS_SCRIPT
        ${CMAKE_CURRENT_LIST_DIR}/PytestAddTests.cmake)
endif()
