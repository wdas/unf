set(CLASSES
    testBroker
    testBrokerBatching
    testBrokerBlocking
    testCache
    testTransaction
)

set(dispatcher_path "${CMAKE_CURRENT_BINARY_DIR}/plugins/*/resources/")

add_subdirectory(plugins)

foreach(NAME IN LISTS CLASSES)
    add_executable(${NAME} "${NAME}.cpp")

    target_link_libraries(${NAME}
        PRIVATE
            UsdNoticeBroker
            GTest::GTest
    )

    add_test(${NAME} ${NAME})

    set_property(
        TEST ${NAME}
        PROPERTY ENVIRONMENT
            PXR_PLUGINPATH_NAME=${dispatcher_path}:$ENV{PXR_PLUGINPATH_NAME})
endforeach()

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
