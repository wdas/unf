cmake_minimum_required(VERSION 3.15)

project(unf LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_PYTHON_BINDINGS "Build Python Bindings" ON)
option(BUNDLE_PYTHON_TESTS "Bundle Python tests per group (faster)" OFF)
option(BUILD_SHARED_LIBS "Build Shared Library" ON)

# Make module finder scripts available for TBB and USD.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# Discover Python per best matching location instead of highest version.
# https://cmake.org/cmake/help/latest/policy/CMP0094.html
cmake_policy(SET CMP0094 NEW)

if(BUILD_PYTHON_BINDINGS)
    # The 'manylinux' images do not include the Python library.
    # CMake >= 3.18 is required for this option to work as expected.
    # https://github.com/pypa/manylinux
    if (TARGET_PYTHON_MODULE OR ($ENV{TARGET_PYTHON_MODULE}))
        find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
        add_library(Python::Python ALIAS Python::Module)
    else()
        find_package(Python COMPONENTS Interpreter Development REQUIRED)
    endif()

    # Convenient variable to fetch module against Python version found.
    set(_py_version ${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
    mark_as_advanced(_py_version)

    find_package(Boost 1.70.0 COMPONENTS "python${_py_version}" REQUIRED)

    # Define generic target for Boost Python if necessary.
    if (NOT TARGET Boost::python)
        add_library(Boost::python ALIAS "Boost::python${_py_version}")
    endif()

    # Set variable to identify the path to install and test python libraries.
    set(PYTHON_VERSION
        "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}"
        CACHE INTERNAL "Python version.")

    set(PYTHON_DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages"
        CACHE INTERNAL "Python library path.")
else()
    find_package(Boost 1.70.0 REQUIRED)
endif()

find_package(USD 0.20.11 REQUIRED)
find_package(TBB 2018.0 COMPONENTS tbb REQUIRED)

add_subdirectory(src)

if (BUILD_TESTS)
    find_package(GTest 1.8.0 REQUIRED)

    # For compatibility with module fetched from CMake < 3.20
    if (NOT TARGET GTest::gtest)
        set_target_properties(GTest::GTest PROPERTIES IMPORTED_GLOBAL TRUE)
        add_library(GTest::gtest ALIAS GTest::GTest)
    endif()
    if (NOT TARGET GTest::gtest_main)
        set_target_properties(GTest::Main PROPERTIES IMPORTED_GLOBAL TRUE)
        add_library(GTest::gtest_main ALIAS GTest::Main)
    endif()

    include(GoogleTest)

    if(BUILD_PYTHON_BINDINGS)
        find_package(Pytest 4.6.11 REQUIRED)
    endif()

    enable_testing()

    add_subdirectory(test)
endif()

if (BUILD_DOCS)
    find_package(Sphinx 1.8.6 REQUIRED)
    find_package(Doxygen 1.8.5 REQUIRED)

    add_subdirectory(doc)
endif()

# Add format target if clang-format is found.
find_package(ClangFormat 7)

include(CMakePackageConfigHelpers)

install(
    FILES
        "cmake/unfConfig.cmake"
    DESTINATION share/cmake/unf
)
