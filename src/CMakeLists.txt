add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
_add_library(
    SHARED 
    UsdNoticeBroker

    SOURCES
        broker.cpp
        dispatcher.cpp
        notice.cpp
        transaction.cpp

    PUBLIC_HEADERS
        broker.h
        dispatcher.h
        notice.h
        transaction.h
        noticeWrapper.h

    INCLUDE_DIRS
        ${USD_INCLUDE_DIR} 
        ${TBB_INCLUDE_DIR} 
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}

    LIBRARIES
        ${USD_LIBRARIES}
)

add_subdirectory(python)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()   
