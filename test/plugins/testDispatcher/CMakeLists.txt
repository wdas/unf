add_library(TestDispatcher
    main.cpp
)

target_link_libraries(TestDispatcher
    PUBLIC
        UsdNoticeBroker
)

configure_file(
    "plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/resources/plugInfo.json")

install(
    TARGETS TestDispatcher
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/testDispatcher
)
install(
    FILES plugInfo.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/testDispatcher/resources
)
