add_library(unf
    unf/broker.cpp
    unf/capturePredicate.cpp
    unf/dispatcher.cpp
    unf/notice.cpp
    unf/transaction.cpp
)

# Suppress warning sfrom USD regarding deprecated TBB features.
# https://github.com/PixarAnimationStudios/OpenUSD/issues/1471
target_compile_definitions(unf
    PUBLIC TBB_SUPPRESS_DEPRECATED_MESSAGES
)

# Suppress warnings from USD library including <ext/hash_set> .
# https://github.com/PixarAnimationStudios/OpenUSD/issues/1057
target_compile_definitions(unf
    PUBLIC _GLIBCXX_PERMIT_BACKWARD_HASH
)

# Ensure that all symbols are exported
target_compile_definitions(unf
    PRIVATE
        UNF_EXPORTS=1
)

target_include_directories(unf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(unf
    PUBLIC
        usd::arch
        usd::plug
        usd::sdf
        usd::tf
        usd::usd
        usd::vt
)

# Transitive Pixar libraries depend on vendorized Boost.Python
# (Required due to manual CMake module used to locate USD)
if (BUILD_PYTHON_BINDINGS)
    target_link_libraries(unf PUBLIC usd::boost usd::python)
endif()

install(
    TARGETS unf
    EXPORT ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY
        unf
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ${PROJECT_NAME}
    FILE unf-targets.cmake
    NAMESPACE unf::
    DESTINATION share/cmake/unf
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
