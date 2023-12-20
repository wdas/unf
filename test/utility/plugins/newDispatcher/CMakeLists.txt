add_library(unfTestNewDispatcher SHARED
    unfTest/newDispatcher/dispatcher.cpp
)

target_compile_definitions(unfTestNewDispatcher
    PRIVATE
        UNF_EXPORTS=1
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

file(
    GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugInfo_$<CONFIG>.json"
    INPUT "plugInfo.json"
)
