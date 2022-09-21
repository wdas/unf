add_library(unfTestNewDispatcher
    unfTest/newDispatcher/dispatcher.cpp
)

target_link_libraries(unfTestNewDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestNewDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

configure_file("plugInfo.json" "${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json")
