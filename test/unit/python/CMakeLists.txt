pytest_discover_tests(
    PythonUnitTests
    LIBRARY_PATH_PREPEND
        $<TARGET_FILE_DIR:unf>
        $<TARGET_FILE_DIR:pyUnf>
    PYTHON_PATH_PREPEND
        "$<TARGET_FILE_DIR:pyUnf>/.."
    TRIM_FROM_NAME "^test_"
)
