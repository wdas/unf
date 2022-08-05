add_test(
    NAME IntegrationTestsPython
    COMMAND ${Python_EXECUTABLE} -m pytest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Ensure that environment is correct for Python tests to run.
set(lib_path "$<TARGET_FILE_DIR:UsdNoticeBroker>")
set(lib_path "${lib_path}:$<TARGET_FILE_DIR:PyUsdNoticeBroker>")

set(python_path "$<TARGET_FILE_DIR:PyUsdNoticeBroker>/..")

set_property(
    TEST IntegrationTestsPython
    PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${lib_path}:$ENV{LD_LIBRARY_PATH})

set_property(
    TEST IntegrationTestsPython
    APPEND PROPERTY ENVIRONMENT PYTHONPATH=${python_path}:$ENV{PYTHONPATH})
