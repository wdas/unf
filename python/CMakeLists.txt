add_library(PyUsdNoticeBroker SHARED
    module.cpp
    wrapBroker.cpp
    wrapNotice.cpp
    wrapTransaction.cpp
)

# Supress warnings from Boost Python regarding deprecated Bind placeholders.
# https://github.com/boostorg/python/issues/359
target_compile_definitions(
    PyUsdNoticeBroker PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)

target_compile_definitions(
    PyUsdNoticeBroker PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES)


target_include_directories(PyUsdNoticeBroker
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(PyUsdNoticeBroker
    PUBLIC
        UsdNoticeBroker
        Boost::python
)

target_link_libraries(PyUsdNoticeBroker
    PRIVATE
        Boost::boost
        Python::Python
        TBB::tbb
)

set_target_properties(PyUsdNoticeBroker PROPERTIES PREFIX "")
set_target_properties(
    PyUsdNoticeBroker PROPERTIES OUTPUT_NAME _usd_notice_broker)

target_compile_definitions(
    PyUsdNoticeBroker
    PRIVATE
        BOOST_PYTHON_NO_PY_SIGNATURES
        PXR_PYTHON_ENABLED=1
        PXR_PYTHON_MODULES_ENABLED=1
        MFB_PACKAGE_MODULE=usd_notice_broker
        MFB_PACKAGE_NAME=usd_notice_broker
        MFB_ALT_PACKAGE_NAME=usd_notice_broker
)

set(PYTHON_VERSION "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")
set(PYTHON_DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages")

install(
    TARGETS PyUsdNoticeBroker
    DESTINATION ${PYTHON_DESTINATION}/usd_notice_broker
)
install(
    FILES __init__.py
    DESTINATION ${PYTHON_DESTINATION}/usd_notice_broker
)
