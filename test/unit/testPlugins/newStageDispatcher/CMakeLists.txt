add_library(unfNewStageDispatcher
    unfTest/newStageDispatcher.cpp
)

target_link_libraries(unfNewStageDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfNewStageDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
