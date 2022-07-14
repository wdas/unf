add_library(TestDispatcher
    main.cpp
)

# Suppress warning sfrom USD regarding deprecated TBB features.
# https://github.com/PixarAnimationStudios/USD/issues/1471
target_compile_definitions(TestDispatcher
    PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES)

# Suppress warnings from USD library including <ext/hash_set> .
# https://github.com/PixarAnimationStudios/USD/issues/1057
target_compile_definitions(TestDispatcher
    PUBLIC _GLIBCXX_PERMIT_BACKWARD_HASH)

target_include_directories(TestDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(TestDispatcher
    PUBLIC
        UsdNoticeBroker
)

target_link_libraries(TestDispatcher
    PRIVATE
        Boost::boost
        Python::Python
        TBB::tbb
)

configure_file(
    "plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/resources/plugInfo.json")

install(
    TARGETS TestDispatcher
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/testDispatcher
)
install(
    FILES plugInfo.json
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/testDispatcher/resources
)
