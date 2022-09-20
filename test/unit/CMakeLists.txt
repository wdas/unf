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
        unfTestNewStageDispatcher
        unfTestNewDispatcher
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitDispatcher)

add_executable(testUnitDispatcherPlugin testDispatcherPlugin.cpp)
target_link_libraries(testUnitDispatcherPlugin
    PRIVATE
        unf
        unfTest
        unfTestNewStageDispatcher
        unfTestNewDispatcher
        GTest::gtest
        GTest::gtest_main
)
set(_path "$<TARGET_FILE_DIR:unfTest>/plugins/*Dispatcher/plugInfo.json")
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

add_executable(testUnitBroadcaster testBroadcaster.cpp)
target_link_libraries(testUnitBroadcaster
    PRIVATE
        unf
        unfTest
        unfTestChildBroadcaster
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(
    testUnitBroadcaster
    PROPERTIES ENVIRONMENT
        "USD_TEST_PATH=${CMAKE_CURRENT_SOURCE_DIR}/testBroadcaster"
)

add_executable(testUnitHierarchyCache testHierarchyCache.cpp)
target_link_libraries(testUnitHierarchyCache
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(
    testUnitHierarchyCache
    PROPERTIES ENVIRONMENT
        "USD_TEST_PATH=${CMAKE_CURRENT_SOURCE_DIR}/testHierarchyCache"
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
