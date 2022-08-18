add_library(UnitPluginNewStageDispatcher
    TestPlugin/NewStageDispatcher.cpp
)

target_link_libraries(UnitPluginNewStageDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(UnitPluginNewStageDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
