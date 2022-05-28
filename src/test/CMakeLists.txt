_add_unit_tests(
    CLASSES
        testBroker
        testBrokerBatching
        testBrokerBlocking
        testCache
        testTransaction

    INCLUDE_DIRS
        ${USD_INCLUDE_DIR} 
        ${GTEST_INCLUDE_DIRS} 
        ${Boost_INCLUDE_DIRS}

    LIBRARIES
        ${GTEST_LIBRARIES}
        ${Python_LIBRARIES}
        ${Boost_LIBRARIES}
        ${USD_LIBRARIES}
        UsdNoticeBroker
)
