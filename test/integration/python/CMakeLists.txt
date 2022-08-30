pytest_discover_tests(
    PythonIntegrationTest
    LIBRARY_PATH_PREPEND
        $<TARGET_FILE_DIR:unf>
        $<TARGET_FILE_DIR:pyUnf>
    PYTHON_PATH_PREPEND
        "$<TARGET_FILE_DIR:pyUnf>/.."
    TRIM_NAME_PREFIX "^test_"
)
