add_executable(testIntegrationAddPrims testAddPrims.cpp)
target_link_libraries(testIntegrationAddPrims
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(testIntegrationAddPrims)

add_executable(testIntegrationMuteLayers testMuteLayers.cpp)
target_link_libraries(testIntegrationMuteLayers
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(testIntegrationMuteLayers)

add_executable(testIntegrationChangeEditTarget testChangeEditTarget.cpp)
target_link_libraries(testIntegrationChangeEditTarget
    PRIVATE
        UsdNoticeBroker
        TestUsdNoticeBroker
        GTest::GTest
        GTest::Main
)
gtest_discover_tests(testIntegrationChangeEditTarget)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
