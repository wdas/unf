add_executable(testBroker testBroker.cpp)
target_link_libraries(testBroker
    PRIVATE
        UsdNoticeBroker
        GTest::GTest
        GTest::Main
)
add_test(testBroker testBroker)

add_executable(testBrokerBlocking testBrokerBlocking.cpp)
target_link_libraries(testBrokerBlocking
    PRIVATE
        UsdNoticeBroker
        GTest::GTest
        GTest::Main
)
add_test(testBrokerBlocking testBrokerBlocking)

add_executable(testBrokerBatching testBrokerBatching.cpp)
target_link_libraries(testBrokerBatching
    PRIVATE
        UsdNoticeBroker
        GTest::GTest
        GTest::Main
)
add_test(testBrokerBatching testBrokerBatching)

add_executable(testCache testCache.cpp)
target_link_libraries(testCache
    PRIVATE
        UsdNoticeBroker
        GTest::GTest
        GTest::Main
)
add_test(testCache testCache)

add_executable(testTransaction testTransaction.cpp)
target_link_libraries(testTransaction
    PRIVATE
        UsdNoticeBroker
        GTest::GTest
        GTest::Main
)
add_test(testTransaction testTransaction)


if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
