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
set(pxr_plug_path $ENV{PXR_PLUGINPATH_NAME})
set(extended_pxr_plug_path "$<TARGET_FILE_DIR:unfTest>/plugins/*Dispatcher/plugInfo.json:${pxr_plug_path}")
if (WIN32)
    string(REPLACE "\\" "\\\\" extended_pxr_plug_path "${extended_pxr_plug_path}")
    string(REPLACE ";" "\;" extended_pxr_plug_path "${extended_pxr_plug_path}")
endif()
gtest_discover_tests(
    testUnitDispatcherPlugin
    PROPERTIES ENVIRONMENT
        "PXR_PLUGINPATH_NAME=${extended_pxr_plug_path}"
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
