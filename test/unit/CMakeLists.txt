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
set(_path "${CMAKE_BINARY_DIR}/test/utility/plugins/*Dispatcher/plugInfo_$<CONFIG>.json")
gtest_discover_tests(
    testUnitDispatcherPlugin
    PROPERTIES ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${_path}$<IF:$<BOOL:${WIN32}>,;,:>$ENV{PXR_PLUGINPATH_NAME}"

)
add_executable(testUnitTransaction testTransaction.cpp)
target_link_libraries(testUnitTransaction
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitTransaction)

add_executable(testUnitObjectsChanged testObjectsChanged.cpp)
target_link_libraries(testUnitObjectsChanged
    PRIVATE
        unf
        unfTest
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(testUnitObjectsChanged)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
