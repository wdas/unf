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
        PUBLIC 
            UsdNoticeBroker
    )

    target_link_libraries(${NAME}
        PRIVATE 
            Boost::boost
            Boost::python
            Python::Python
            GTest::GTest
    )

    add_test(${NAME} ${NAME})
endforeach()
