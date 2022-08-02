add_library(TestBroadcaster2
    main.cpp
)

target_link_libraries(TestBroadcaster2
    PUBLIC
        UsdNoticeBroker
)

configure_file(
    "plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/resources/plugInfo.json")

install(
    TARGETS TestBroadcaster2
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/TestBroadcaster2
)
install(
    FILES plugInfo.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/TestBroadcaster2/resources
)
