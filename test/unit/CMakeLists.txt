add_executable(testUnitBroker testBroker.cpp)
target_link_libraries(testUnitBroker
    PRIVATE
        unf
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBroker)

add_executable(testUnitBrokerFlow testBrokerFlow.cpp)
target_link_libraries(testUnitBrokerFlow
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitBrokerFlow)

add_executable(testUnitDispatcher testDispatcher.cpp)
target_link_libraries(testUnitDispatcher
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitDispatcher)

add_executable(testUnitDispatcherPlugin testDispatcherPlugin.cpp)
target_link_libraries(testUnitDispatcherPlugin
    PRIVATE
        unf
        unfTest
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
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitCache)

add_executable(testUnitTransaction testTransaction.cpp)
target_link_libraries(testUnitTransaction
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitTransaction)

add_executable(testHierarchyCache testHierarchyCache.cpp)
target_link_libraries(testHierarchyCache
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(
    testHierarchyCache
    PROPERTIES ENVIRONMENT
        "USD_TEST_PATH=${CMAKE_CURRENT_SOURCE_DIR}/testHierarchyCache"
)

add_executable(testBroadcaster testBroadcaster.cpp)
target_link_libraries(testBroadcaster
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(
    testBroadcaster
    PROPERTIES ENVIRONMENT
        "USD_TEST_PATH=${CMAKE_CURRENT_SOURCE_DIR}/testBroadcaster"
)
add_subdirectory(plugins)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
