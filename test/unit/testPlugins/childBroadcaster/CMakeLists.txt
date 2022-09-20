add_library(unfChildBroadcaster
    unfTest/childBroadcaster.cpp
)

target_link_libraries(unfChildBroadcaster
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfChildBroadcaster
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
