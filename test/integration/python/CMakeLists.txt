pytest_discover_tests(
    PythonIntegrationTest
    LIBRARY_PATH_PREPEND
        $<TARGET_FILE_DIR:unf>
        $<TARGET_FILE_DIR:pyUnf>
    PYTHON_PATH_PREPEND
        "$<TARGET_FILE_DIR:pyUnf>/.."
    TRIM_FROM_NAME "^test_"
    BUNDLE_TESTS ${BUNDLE_PYTHON_TESTS}
    DEPENDS unf pyUnf
)
