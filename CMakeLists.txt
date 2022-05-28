cmake_minimum_required(VERSION 3.15)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

project(UsdNoticeBroker VERSION 0.1.0)

find_package(Python 2.7 COMPONENTS Interpreter Development REQUIRED)
find_package(Boost 1.70.0 COMPONENTS python REQUIRED)
find_package(TBB 2020.3 EXACT REQUIRED)
find_package(USD 0.20.11 REQUIRED)

include(Config)

if (BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
endif()   

add_subdirectory(src)
