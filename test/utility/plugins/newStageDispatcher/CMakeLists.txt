add_library(unfTestNewStageDispatcher SHARED
    unfTest/newStageDispatcher/dispatcher.cpp
)

target_compile_definitions(unfTestNewStageDispatcher
    PRIVATE
        UNF_EXPORTS=1
)

target_link_libraries(unfTestNewStageDispatcher
    PUBLIC
        unf
        unfTest
)

target_include_directories(unfTestNewStageDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

file(
    GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugInfo_$<CONFIG>.json"
    INPUT "plugInfo.json"
)
