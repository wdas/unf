if (WIN32)
    # Monkeypatch Gtest discovery function to extend environment so that
    # relevant DLLs can be found on Windows. We use 'set_tests_properties'
    # to prevent issue with escaped semi-colons when passing environment
    # to 'gtest_discover_tests'.
    macro(gtest_discover_tests NAME)
        gtest_add_tests(TARGET ${NAME} TEST_LIST tests)

        # Extract and sanitize 'PATH' environment variable.
        string(REPLACE ";" "\;" env_path "$ENV{PATH}")

        # Extract and sanitize environment passed to function.
        cmake_parse_arguments("" "" "" "PROPERTIES" ${ARGN})
        cmake_parse_arguments("" "" "" "ENVIRONMENT" ${_PROPERTIES})
        string(REPLACE ";" "\;" extra_env "${_ENVIRONMENT}")

        # Apply environment to newly added tests.
        set(env_vars
            "PATH=$<JOIN:$<TARGET_RUNTIME_DLL_DIRS:${NAME}>,\\;>\;${env_path}"
            ${extra_env})
        set_tests_properties(${tests} PROPERTIES ENVIRONMENT "${env_vars}")
    endmacro()
endif()

add_subdirectory(utility)
add_subdirectory(unit)
add_subdirectory(integration)
