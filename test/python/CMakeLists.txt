add_subdirectory(utility)

add_test(
    NAME testPython
    COMMAND ${Python_EXECUTABLE} -m pytest
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/python
)

set(lib_path "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(python_path "${CMAKE_INSTALL_PREFIX}/${PYTHON_DESTINATION}")

set_property(
    TEST testPython
    PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${lib_path}:$ENV{LD_LIBRARY_PATH})

set_property(
    TEST testPython
    APPEND PROPERTY ENVIRONMENT PYTHONPATH=${python_path}:$ENV{PYTHONPATH})
