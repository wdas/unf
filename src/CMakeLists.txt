# _add_library(
#     SHARED 
#     USDNoticeBroker

#     ...
# )

add_subdirectory(python)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()   
