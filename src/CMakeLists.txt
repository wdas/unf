add_library(UsdNoticeBroker
    broker.cpp
    dispatcher.cpp
    notice.cpp
    transaction.cpp
)

# Suppress warning sfrom USD regarding deprecated TBB features.
# https://github.com/PixarAnimationStudios/USD/issues/1471
target_compile_definitions(UsdNoticeBroker
    PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES)

# Suppress warnings from USD library including <ext/hash_set> .
# https://github.com/PixarAnimationStudios/USD/issues/1057
target_compile_definitions(UsdNoticeBroker
    PUBLIC _GLIBCXX_PERMIT_BACKWARD_HASH)

target_include_directories(UsdNoticeBroker
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(UsdNoticeBroker
    PUBLIC
        pxr::arch
        pxr::sdf
        pxr::tf
        pxr::usd
)

target_link_libraries(UsdNoticeBroker
    PRIVATE
        Boost::boost
        Python::Python
        TBB::tbb
)

install(TARGETS UsdNoticeBroker EXPORT ${PROJECT_NAME})
install(
    FILES
        broker.h
        dispatcher.h
        notice.h
        pyNoticeWrapper.h
        transaction.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/UsdNoticeBroker
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
