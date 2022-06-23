cmake_minimum_required(VERSION 3.15)

project(UsdNoticeBroker LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Discover Python per best matching location instead of highest version.
# https://cmake.org/cmake/help/latest/policy/CMP0094.html
cmake_policy(SET CMP0094 NEW)

find_package(USD 0.20.11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.70.0 COMPONENTS python REQUIRED)
find_package(TBB 2020.3 REQUIRED)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_PYTHON_BINDINGS "Build Python Bindings" ON)
option(BUILD_SHARED_LIBS "Build Shared Library" ON)

add_subdirectory(src)

if (BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()

    add_subdirectory(test)
endif()   

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()   
