add_library(unfTest
    unfTest/notice.cpp
)

target_compile_definitions(unfTest
    PRIVATE
        UNF_EXPORTS=1
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
