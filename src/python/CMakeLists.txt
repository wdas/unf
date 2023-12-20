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
    pyUnf PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS
)

target_compile_definitions(
    pyUnf PRIVATE BOOST_PYTHON_NO_PY_SIGNATURES
)

set_target_properties(pyUnf
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/src/python/$<CONFIG>/unf"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/src/python/$<CONFIG>/unf"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/src/python/$<CONFIG>/unf"
)

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

if(WIN32)
    # Python modules must be suffixed with .pyd on Windows.
    set_target_properties(pyUnf
        PROPERTIES
            SUFFIX ".pyd"
    )
endif()

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

file(
    GENERATE
    OUTPUT "$<TARGET_FILE_DIR:pyUnf>/__init__.py"
    INPUT "__init__.py"
)

install(
    TARGETS pyUnf
    LIBRARY DESTINATION ${PYTHON_DESTINATION}/unf
    RUNTIME DESTINATION ${PYTHON_DESTINATION}/unf
)
install(
    FILES __init__.py
    DESTINATION ${PYTHON_DESTINATION}/unf
)
