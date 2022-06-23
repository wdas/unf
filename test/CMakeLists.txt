set(CLASSES
    testBroker
    testBrokerBatching
    testBrokerBlocking
    testCache
    testTransaction
)

foreach(NAME IN LISTS CLASSES)
    add_executable(${NAME} "${NAME}.cpp")

    target_link_libraries(${NAME}
        PRIVATE
            UsdNoticeBroker
            Boost::boost
            Boost::python
            Python::Python
            tbb::tbb
            GTest::GTest
    )

    add_test(${NAME} ${NAME})
endforeach()
