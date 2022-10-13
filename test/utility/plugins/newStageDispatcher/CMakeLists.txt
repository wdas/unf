add_library(unfTestNewStageDispatcher
    unfTest/newStageDispatcher/dispatcher.cpp
)

target_link_libraries(unfTestNewStageDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestNewStageDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
