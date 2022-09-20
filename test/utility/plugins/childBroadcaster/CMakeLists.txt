add_library(unfTestChildBroadcaster
    unfTest/childBroadcaster/broadcaster.cpp
)

target_link_libraries(unfTestChildBroadcaster
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestChildBroadcaster
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
