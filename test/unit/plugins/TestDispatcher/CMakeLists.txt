add_library(UnitPluginTestDispatcher
    TestPlugin/TestDispatcher.cpp
)

target_link_libraries(UnitPluginTestDispatcher
    PUBLIC
        UsdNoticeBroker
        TestUsdNoticeBroker
)

target_include_directories(UnitPluginTestDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
