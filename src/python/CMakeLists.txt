_add_python_binding(
    usd_notice_broker

    SOURCES
        module.cpp
        wrapBroker.cpp
        wrapNotice.cpp
        wrapTransaction.cpp

    LIBRARIES
        ${USD_LIBRARIES}
        UsdNoticeBroker

    INCLUDE_DIRS
        ${USD_INCLUDE_DIR} 
        ${TBB_INCLUDE_DIR} 
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}

    PYTHON_SOURCES
        __init__.py
)
