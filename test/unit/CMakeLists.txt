add_executable(testUnitBroker testBroker.cpp)
target_link_libraries(testUnitBroker
    PRIVATE
        UsdNoticeBroker
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBroker)

add_executable(testUnitBrokerFlow testBrokerFlow.cpp)
target_link_libraries(testUnitBrokerFlow
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBrokerFlow)

add_executable(testUnitDispatcher testDispatcher.cpp)
target_link_libraries(testUnitDispatcher
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitDispatcher)

add_executable(testUnitDispatcherPlugin testDispatcherPlugin.cpp)
target_link_libraries(testUnitDispatcherPlugin
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        UnitPluginNewStageDispatcher
        UnitPluginTestDispatcher
        GTest::gtest
        GTest::gtest_main
)
set(_path "${CMAKE_CURRENT_BINARY_DIR}/plugins/*Dispatcher/plugInfo.json")
gtest_discover_tests(
    testUnitDispatcherPlugin
    PROPERTIES ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${_path}:$ENV{PXR_PLUGINPATH_NAME}"
)

add_executable(testUnitCache testCache.cpp)
target_link_libraries(testUnitCache
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitCache)

add_executable(testUnitTransaction testTransaction.cpp)
target_link_libraries(testUnitTransaction
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitTransaction)

add_subdirectory(plugins)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
