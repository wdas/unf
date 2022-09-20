add_library(unfTestDispatcher
    unfTest/testDispatcher.cpp
)

target_link_libraries(unfTestDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
