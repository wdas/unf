# Discover required Clang-format target.
#
# This module defines the following imported targets:
#     Clangformat
#
# It also create the target 'format' to apply clang-format rules to project.
#
# Usage:
#     find_package(ClangFormat)
#     find_package(ClangFormat REQUIRED)
#     find_package(ClangFormat 7.0.1 REQUIRED)
#
# Note:
#     The ClangFormat_ROOT environment variable or CMake variable can be used to
#     prepend a custom search path.
#     (https://cmake.org/cmake/help/latest/policy/CMP0074.html)

include(FindPackageHandleStandardArgs)

find_program(CLANG_FORMAT NAMES clang-format)

if(CLANG_FORMAT)
    execute_process(
        COMMAND "${CLANG_FORMAT}" --version
        OUTPUT_VARIABLE _version
        ERROR_VARIABLE _version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_version MATCHES "version ([0-9]+)")
        set(CLANG_FORMAT_VERSION "${CMAKE_MATCH_1}")
    endif()

    mark_as_advanced(_version)
endif()

mark_as_advanced(CLANG_FORMAT CLANG_FORMAT_VERSION)

find_package_handle_standard_args(
    ClangFormat
    REQUIRED_VARS
        CLANG_FORMAT
    VERSION_VAR
        CLANG_FORMAT_VERSION
)

if (ClangFormat_FOUND AND NOT TARGET ClangFormat)
    add_executable(ClangFormat IMPORTED GLOBAL)
    set_target_properties(ClangFormat PROPERTIES
        IMPORTED_LOCATION "${CLANG_FORMAT}"
    )

    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/*.cpp
        ${PROJECT_SOURCE_DIR}/*.h)

    add_custom_target(
        format
        COMMAND ClangFormat -i ${ALL_SOURCE_FILES}
        COMMENT "Apply clang-format to project"
    )
endif()
