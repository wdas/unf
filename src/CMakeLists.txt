_add_library(
    SHARED 
    UsdNoticeBroker

    SOURCES
        broker.cpp
        notice.cpp

    PUBLIC_HEADERS
        broker.h
        notice.h

    INCLUDE_DIRS
        ${USD_INCLUDE_DIR} 
        ${TBB_INCLUDE_DIR} 
        ${Boost_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}

    LIBRARIES
        ${USD_LIBRARIES}
)

add_subdirectory(python)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()   
