add_test(
    NAME UnitTestsPython
    COMMAND ${Python_EXECUTABLE} -m pytest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Ensure that environment is correct for Python tests to run.
set(lib_path "$<TARGET_FILE_DIR:UsdNoticeBroker>")
set(lib_path "${lib_path}:$<TARGET_FILE_DIR:PyUsdNoticeBroker>")
set(lib_path "${lib_path}:$<TARGET_FILE_DIR:PyUsdNoticeBrokerTest>")

set(python_path "$<TARGET_FILE_DIR:PyUsdNoticeBroker>/..")
set(python_path "${python_path}:$<TARGET_FILE_DIR:PyUsdNoticeBrokerTest>/..")

set_property(
    TEST UnitTestsPython
    PROPERTY ENVIRONMENT
    LD_LIBRARY_PATH=${lib_path}:$ENV{LD_LIBRARY_PATH})

set_property(
    TEST UnitTestsPython
    APPEND PROPERTY ENVIRONMENT PYTHONPATH=${python_path}:$ENV{PYTHONPATH})
