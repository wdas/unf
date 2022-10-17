add_library(unfTest
    unfTest/notice.cpp
)

target_include_directories(unfTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(unfTest
    PUBLIC
        unf
)

add_subdirectory(plugins)
