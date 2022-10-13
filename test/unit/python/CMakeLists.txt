pytest_discover_tests(
    PythonUnitTests
    LIBRARY_PATH_PREPEND
        $<TARGET_FILE_DIR:unf>
        $<TARGET_FILE_DIR:pyUnf>
        $<TARGET_FILE_DIR:pyUnfTest>
    PYTHON_PATH_PREPEND
        "$<TARGET_FILE_DIR:pyUnf>/.."
        "$<TARGET_FILE_DIR:pyUnfTest>/.."
    TRIM_FROM_NAME "^test_"
    BUNDLE_TESTS ${BUNDLE_PYTHON_TESTS}
)
