add_library(pyUnfTest SHARED
    module.cpp
    wrapNotice.cpp
)

# Supress warnings from Boost Python regarding deprecated Bind placeholders.
# https://github.com/boostorg/python/issues/359
target_compile_definitions(
    pyUnfTest PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)

target_compile_definitions(
    pyUnfTest PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES)


target_include_directories(pyUnfTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(pyUnfTest
    PUBLIC
        unf
        Boost::python
)

target_link_libraries(pyUnfTest
    PRIVATE
        unfTest
)

set_target_properties(pyUnfTest
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME _usd_notice_framework_test
        LIBRARY_OUTPUT_DIRECTORY usd_notice_framework_test
)

target_compile_definitions(
    pyUnfTest
    PRIVATE
        BOOST_PYTHON_NO_PY_SIGNATURES
        PXR_PYTHON_ENABLED=1
        PXR_PYTHON_MODULES_ENABLED=1
        MFB_PACKAGE_MODULE=usd_notice_framework_test
        MFB_PACKAGE_NAME=usd_notice_framework_test
        MFB_ALT_PACKAGE_NAME=usd_notice_framework_test
)

configure_file(
    "__init__.py"
    "${CMAKE_CURRENT_BINARY_DIR}/usd_notice_framework_test/__init__.py")
