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

add_executable(testUnitNoticeCache testNoticeCache.cpp)
target_link_libraries(testUnitNoticeCache
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitNoticeCache)

add_executable(testUnitTransaction testTransaction.cpp)
target_link_libraries(testUnitTransaction
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitTransaction)

add_executable(testUnitHierarchyBroadcaster testHierarchyBroadcaster.cpp)
target_link_libraries(testUnitHierarchyBroadcaster
    PRIVATE
        unf
        unfHierarchyBroadcaster
        unfTest
        unfTestChildBroadcaster
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(
    testUnitHierarchyBroadcaster
    PROPERTIES ENVIRONMENT
        "USD_TEST_PATH=${CMAKE_CURRENT_SOURCE_DIR}/testHierarchyBroadcaster"
)

add_executable(testUnitHierarchyCache testHierarchyCache.cpp)
target_link_libraries(testUnitHierarchyCache
    PRIVATE
        unf
        unfHierarchyBroadcaster
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
