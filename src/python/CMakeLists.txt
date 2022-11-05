add_library(pyUnf SHARED
    module.cpp
    wrapBroker.cpp
    wrapCapturePredicate.cpp
    wrapNotice.cpp
    wrapTransaction.cpp
)

# Supress warnings from Boost Python regarding deprecated Bind placeholders.
# https://github.com/boostorg/python/issues/359
target_compile_definitions(
    pyUnf PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)

target_compile_definitions(
    pyUnf PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES)


target_include_directories(pyUnf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(pyUnf
    PUBLIC
        unf
        Boost::python
)

set_target_properties(pyUnf
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME _unf
        LIBRARY_OUTPUT_DIRECTORY unf
)

target_compile_definitions(
    pyUnf
    PRIVATE
        BOOST_PYTHON_NO_PY_SIGNATURES
        PXR_PYTHON_ENABLED=1
        PXR_PYTHON_MODULES_ENABLED=1
        MFB_PACKAGE_MODULE=unf
        MFB_PACKAGE_NAME=unf
        MFB_ALT_PACKAGE_NAME=unf
)

configure_file("__init__.py" "${CMAKE_CURRENT_BINARY_DIR}/unf/__init__.py")

install(
    TARGETS pyUnf
    DESTINATION ${PYTHON_DESTINATION}/unf
)
install(
    FILES __init__.py
    DESTINATION ${PYTHON_DESTINATION}/unf
)
