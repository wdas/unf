add_library(unfTest
    unfTest/testNotice.cpp
)

target_include_directories(unfTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(unfTest
    PUBLIC
        unf
        unfHierarchyBroadcaster
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
