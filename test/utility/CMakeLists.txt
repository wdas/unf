add_library(TestUsdNoticeBroker
    TestUsdNoticeBroker/testNotice.cpp
)

target_include_directories(TestUsdNoticeBroker
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(TestUsdNoticeBroker
    PUBLIC
        UsdNoticeBroker
)

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
