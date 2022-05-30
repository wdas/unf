_add_unit_tests(
    CLASSES
        testBroker
        testBrokerBatching
        testBrokerBlocking
        testCache
        testTransaction

    INCLUDE_DIRS
        ${GTEST_INCLUDE_DIRS} 
        ${USD_INCLUDE_DIR} 
        ${TBB_INCLUDE_DIR} 
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}

    LIBRARIES
        ${USD_LIBRARIES}
        GTest::GTest
        Python::Python
        Boost::python
        UsdNoticeBroker
)
