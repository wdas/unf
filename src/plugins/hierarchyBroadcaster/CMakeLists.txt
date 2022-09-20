add_library(unfHierarchyBroadcaster
    unf/hierarchyBroadcaster/broadcaster.cpp
    unf/hierarchyBroadcaster/cache.cpp
    unf/hierarchyBroadcaster/notice.cpp
)

target_link_libraries(unfHierarchyBroadcaster
    PUBLIC
        unf
)

target_include_directories(unfHierarchyBroadcaster
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")

set(PLUGIN_DESTINATION "${CMAKE_INSTALL_LIBDIR}/plugins/hierarchyBroadcaster")

install(
    TARGETS unfHierarchyBroadcaster
    DESTINATION ${PLUGIN_DESTINATION}
)
install(
    FILES plugInfo.json
    DESTINATION ${PLUGIN_DESTINATION}
)
install(
    DIRECTORY
        unf/hierarchyBroadcaster
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/unf
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ${PROJECT_NAME}
    FILE unfHierarchyBroadcasterTargets.cmake
    DESTINATION share/cmake/unf
)
