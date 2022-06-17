cmake_minimum_required(VERSION 3.15)

project(UsdNoticeBroker VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(USD 0.20.11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.70.0 COMPONENTS python REQUIRED)

include(GNUInstallDirs)

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
