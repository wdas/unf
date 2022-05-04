cmake_minimum_required(VERSION 3.12)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

project(UsdNoticeBroker VERSION 0.1.0)

include(Config)

if (BUILD_TESTS)
    find_package(GTest)
    enable_testing()
endif()   

add_subdirectory(src)
