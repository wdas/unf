cmake_minimum_required(VERSION 3.15)

project(UsdNoticeBroker LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_PYTHON_BINDINGS "Build Python Bindings" ON)
option(BUILD_SHARED_LIBS "Build Shared Library" ON)

# Make module finder scripts available for TBB and USD.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Discover Python per best matching location instead of highest version.
# https://cmake.org/cmake/help/latest/policy/CMP0094.html
cmake_policy(SET CMP0094 NEW)

if(BUILD_PYTHON_BINDINGS)
    # The 'manylinux' images do not include the Python library.
    # CMake >= 3.18 is required for this option to work as expected.
    # https://github.com/pypa/manylinux
    if (TARGET_PYTHON_MODULE OR ($ENV{TARGET_PYTHON_MODULE}))
        find_package(Python COMPONENTS Development.Module REQUIRED)
        add_library(Python::Python ALIAS Python::Module)
    else()
        find_package(Python COMPONENTS Development REQUIRED)
    endif()

    # Convenient variable to fetch module against Python version found.
    set(_py_version ${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
    mark_as_advanced(_py_version)

    find_package(Boost 1.70.0 COMPONENTS "python${_py_version}" REQUIRED)

    # Define generic target for Boost Python if necessary.
    if (NOT TARGET Boost::python)
        add_library(Boost::python ALIAS "Boost::python${_py_version}")
    endif()
endif()

find_package(USD 0.20.11 REQUIRED)
find_package(TBB 2018.0 COMPONENTS tbb REQUIRED)

add_subdirectory(src)

if (BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()

    add_subdirectory(test)
endif()

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
