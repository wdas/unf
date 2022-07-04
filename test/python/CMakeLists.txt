add_test(
    NAME testPython
    COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/python
)
