add_library(TestBroadcaster
    main.cpp
)

target_link_libraries(TestBroadcaster
    PUBLIC
        UsdNoticeBroker
)

configure_file(
    "plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/resources/plugInfo.json")

install(
    TARGETS TestBroadcaster
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/TestBroadcaster
)
install(
    FILES plugInfo.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/TestBroadcaster/resources
)
