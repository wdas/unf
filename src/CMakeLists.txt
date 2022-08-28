add_library(unf
    unf/broadcaster.cpp
    unf/broker.cpp
    unf/dispatcher.cpp
    unf/merger.cpp
    unf/notice.cpp
    unf/transaction.cpp
    unf/hierarchycache.cpp
)

# Suppress warning sfrom USD regarding deprecated TBB features.
# https://github.com/PixarAnimationStudios/USD/issues/1471
target_compile_definitions(unf
    PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES)

# Suppress warnings from USD library including <ext/hash_set> .
# https://github.com/PixarAnimationStudios/USD/issues/1057
target_compile_definitions(unf
    PUBLIC _GLIBCXX_PERMIT_BACKWARD_HASH)

target_include_directories(unf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(unf
    PUBLIC
        pxr::arch
        pxr::plug
        pxr::sdf
        pxr::tf
        pxr::usd
        pxr::vt
)

install(TARGETS unf EXPORT ${PROJECT_NAME})
install(
    DIRECTORY
        unf
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
