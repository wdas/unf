# Wrapper used to create individual CTest tests from Pytest tests.
cmake_minimum_required(VERSION ${CMAKE_VERSION})

if(CMAKE_SCRIPT_MODE_FILE)
    set(libpath $ENV{LD_LIBRARY_PATH})
    set(pythonpath $ENV{PYTHONPATH})

    if (LIBRARY_PATH_PREPEND)
        list(REVERSE "${LIBRARY_PATH_PREPEND}")
        foreach (_path ${LIBRARY_PATH_PREPEND})
            set(libpath "${_path}:${libpath}")
        endforeach()
    endif()

    if (PYTHON_PATH_PREPEND)
        list(REVERSE "${PYTHON_PATH_PREPEND}")
        foreach (_path ${PYTHON_PATH_PREPEND})
            set(pythonpath "${_path}:${pythonpath}")
        endforeach()
    endif()

    # Set environment for collecting tests.
    set(ENV{LD_LIBRARY_PATH} ${libpath})
    set(ENV{PYTHONPATH} ${pythonpath})

    execute_process(
        COMMAND ${PYTEST_EXECUTABLE} --collect-only -q
        OUTPUT_VARIABLE _output_list
        ERROR_VARIABLE _output_list
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${WORKING_DIRECTORY}
    )

    # Convert output into list.
    string(REPLACE [[;]] [[\;]] _output_list "${_output_list}")
    string(REPLACE "\n" ";" _output_list "${_output_list}")

    set(test_pattern "([^:]+)(::([^:]+))?::([^:]+)")

    # Set Cmake test file to execute each test.
    set(_content "")

    foreach (test_case ${_output_list})
        string(REGEX MATCHALL ${test_pattern} _test_case "${test_case}")

        # Ignore lines not identified as a test.
        if (NOT _test_case)
            continue()
        endif()

        set(_class ${CMAKE_MATCH_3})
        set(_func ${CMAKE_MATCH_4})

        if (_class)
            set(test_name "${_class}.${_func}")
        else()
            set(test_name "${_func}")
        endif()

        if (TRIM_FROM_NAME)
            string(REGEX REPLACE
                "${TRIM_FROM_NAME}" "" test_name "${test_name}")
        endif()

        set(test_name "${PREFIX_NAME}.${test_name}")
        set(test_case "${PROJECT_SOURCE_DIR}/${test_case}")

        string(APPEND _content
            "add_test(\n"
            "    \"${test_name}\"\n"
            "    ${PYTEST_EXECUTABLE} ${test_case}\n"
            ")\n"
            "set_tests_properties(\n"
            "     \"${test_name}\" PROPERTIES\n"
            "     ENVIRONMENT LD_LIBRARY_PATH=${libpath}\n"
            ")\n"
            "set_tests_properties(\n"
            "     \"${test_name}\"\n"
            "     APPEND PROPERTIES\n"
            "     ENVIRONMENT PYTHONPATH=${pythonpath}\n"
            ")\n"
        )
    endforeach()

    file(WRITE ${CTEST_FILE} ${_content})
endif()
