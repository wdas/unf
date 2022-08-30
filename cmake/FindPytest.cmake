# Discover required Pytest target.
#
# This module defines the following imported targets:
#     Pytest::Pytest
#
# It also exposes the 'pytest_discover_tests' function which adds ctest
# for each pytest tests.
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

if(PYTEST_EXECUTABLE)
    execute_process(
        COMMAND "${PYTEST_EXECUTABLE}" --version
        OUTPUT_VARIABLE _version
        ERROR_VARIABLE _version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_version MATCHES "pytest version ([0-9]+\\.[0-9]+\\.[0-9]+),")
        set(PYTEST_VERSION "${CMAKE_MATCH_1}")
    endif()

    mark_as_advanced(_version)
endif()

mark_as_advanced(PYTEST_EXECUTABLE PYTEST_VERSION)

find_package_handle_standard_args(
    Pytest
    REQUIRED_VARS
        PYTEST_EXECUTABLE
    VERSION_VAR
        PYTEST_VERSION
)

if (Pytest_FOUND AND NOT TARGET Pytest::Pytest)
    add_executable(Pytest::Pytest IMPORTED GLOBAL)
    set_target_properties(Pytest::Pytest PROPERTIES
        IMPORTED_LOCATION "${PYTEST_EXECUTABLE}"
    )

    function(pytest_discover_tests NAME)
        cmake_parse_arguments(
            PARSE_ARGV 1 "" ""
            "WORKING_DIRECTORY;TRIM_NAME_PREFIX"
            "LIBRARY_PATH_PREPEND;PYTHON_PATH_PREPEND"
        )

        if (NOT _WORKING_DIRECTORY)
            set(_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif()

        set(_tests_file "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_tests.cmake")

        add_custom_target(
            ${NAME} ALL VERBATIM
            BYPRODUCTS "${_tests_file}"
            COMMAND ${CMAKE_COMMAND}
                -DPYTEST_EXECUTABLE=${PYTEST_EXECUTABLE}
                -DPREFIX_NAME=${NAME}
                -DLIBRARY_PATH_PREPEND=${_LIBRARY_PATH_PREPEND}
                -DPYTHON_PATH_PREPEND=${_PYTHON_PATH_PREPEND}
                -DTRIM_NAME_PREFIX=${_TRIM_NAME_PREFIX}
                -DWORKING_DIRECTORY=${_WORKING_DIRECTORY}
                -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
                -DCTEST_FILE=${_tests_file}
                -P ${_PYTEST_DISCOVER_TESTS_SCRIPT}
        )

        # Add discovered tests to directory TEST_INCLUDE_FILES
        set_property(DIRECTORY
            APPEND PROPERTY TEST_INCLUDE_FILES "${_tests_file}"
        )
    endfunction()

    set(_PYTEST_DISCOVER_TESTS_SCRIPT
        ${CMAKE_CURRENT_LIST_DIR}/PytestAddTests.cmake)
endif()
