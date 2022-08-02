add_library(PyUsdNoticeBrokerTest SHARED
    module.cpp
    wrapNotice.cpp
)

# Supress warnings from Boost Python regarding deprecated Bind placeholders.
# https://github.com/boostorg/python/issues/359
target_compile_definitions(
    PyUsdNoticeBrokerTest PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)

target_compile_definitions(
    PyUsdNoticeBrokerTest PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES)


target_include_directories(PyUsdNoticeBrokerTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(PyUsdNoticeBrokerTest
    PUBLIC
        UsdNoticeBroker
        Boost::python
)

set_target_properties(PyUsdNoticeBrokerTest
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME _usd_notice_broker_test
)

target_compile_definitions(
    PyUsdNoticeBrokerTest
    PRIVATE
        BOOST_PYTHON_NO_PY_SIGNATURES
        PXR_PYTHON_ENABLED=1
        PXR_PYTHON_MODULES_ENABLED=1
        MFB_PACKAGE_MODULE=usd_notice_broker_test
        MFB_PACKAGE_NAME=usd_notice_broker_test
        MFB_ALT_PACKAGE_NAME=usd_notice_broker_test
)

install(
    TARGETS PyUsdNoticeBrokerTest
    DESTINATION ${PYTHON_DESTINATION}/usd_notice_broker_test
)
install(
    FILES __init__.py
    DESTINATION ${PYTHON_DESTINATION}/usd_notice_broker_test
)
