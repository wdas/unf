add_library(TestBroadcaster
    main.cpp
)

# Suppress warning sfrom USD regarding deprecated TBB features.
# https://github.com/PixarAnimationStudios/USD/issues/1471
target_compile_definitions(TestBroadcaster
    PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES)

# Suppress warnings from USD library including <ext/hash_set> .
# https://github.com/PixarAnimationStudios/USD/issues/1057
target_compile_definitions(TestBroadcaster
    PUBLIC _GLIBCXX_PERMIT_BACKWARD_HASH)

target_include_directories(TestBroadcaster
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(TestBroadcaster
    PUBLIC
        UsdNoticeBroker
)

target_link_libraries(TestBroadcaster
    PRIVATE
        Boost::boost
        Python::Python
        TBB::tbb
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
