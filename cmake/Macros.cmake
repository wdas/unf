set(_LIBRARY_OPTIONS
    NO_PREFIX
)
set(_LIBRARY_SINGLE_VALUES
    DESTINATION
    HEADER_DESTINATION
    SUFFIX
)
set(_LIBRARY_MULTI_VALUES
    CLASSES
    INCLUDE_DIRS
    LIBRARIES
    PRIVATE_LIBRARIES
    PUBLIC_HEADERS
    PYTHON_SOURCES
    SOURCES
)

function(_add_library TYPE NAME) 
    set(optional ${_LIBRARY_OPTIONS})
    set(single_value ${_LIBRARY_SINGLE_VALUES})
    set(multi_value ${_LIBRARY_MULTI_VALUES})

    cmake_parse_arguments(
        PARSE_ARGV 1 args 
        "${optional}" 
        "${single_value}" 
        "${multi_value}"
    )

    add_library(${NAME} ${TYPE} ${args_SOURCES})

    # Remove library prefix if required.
    if (args_NO_PREFIX)
        set_target_properties(${NAME} PROPERTIES PREFIX "")
    endif()

    # Use custom suffix if required.
    if (args_SUFFIX)
        set_target_properties(${NAME} PROPERTIES SUFFIX "${args_SUFFIX}")
    endif()

    if (args_INCLUDE_DIRS)
        target_include_directories(${NAME} PRIVATE ${args_INCLUDE_DIRS})
    endif()

    target_include_directories(
        ${NAME} PUBLIC 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    if (args_LIBRARIES)
        target_link_libraries(${NAME} PUBLIC ${args_LIBRARIES})
    endif()

    if (args_PRIVATE_LIBRARIES)
        target_link_libraries(${NAME} PRIVATE ${args_PRIVATE_LIBRARIES})
    endif()

    # Set default library destination if none is specified.
    if (NOT DEFINED args_DESTINATION)
        set(args_DESTINATION ${INSTALL_LIBDIR})
    endif()

    # Set default header destination if none is specified.
    if (NOT DEFINED args_HEADER_DESTINATION)
        set(args_HEADER_DESTINATION ${INSTALL_INCLUDEDIR})
    endif()

    # Install public headers
    if (args_PUBLIC_HEADERS)
        install(
            FILES ${args_PUBLIC_HEADERS}
            DESTINATION "${args_HEADER_DESTINATION}/${NAME}"
        )
    endif()

    # Install the library
    install(
        TARGETS ${NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${args_DESTINATION}
    )
endfunction() # _add_library

function(_add_python_binding NAME) 
    set(optional "")
    set(single_value ${_LIBRARY_SINGLE_VALUES})
    set(multi_value ${_LIBRARY_MULTI_VALUES})

    cmake_parse_arguments(
        PARSE_ARGV 1 args 
        "${optional}" 
        "${single_value}" 
        "${multi_value}"
    )

    # Set default library destination if none is specified.
    if (NOT DEFINED args_DESTINATION)
        set(args_DESTINATION ${INSTALL_PYTHONDIR})
    endif()

    # Set the destination as a Python module
    set(args_DESTINATION "${args_DESTINATION}/${NAME}")

    _add_library(
        SHARED ${NAME} NO_PREFIX
        DESTINATION ${args_DESTINATION}
        INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
        SOURCES ${args_SOURCES}
    )

    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "_${NAME}")

    if (args_INCLUDE_DIRS)
        target_include_directories(${NAME} PUBLIC ${args_INCLUDE_DIRS})
    endif()

    if (args_LIBRARIES)
        target_link_libraries(${NAME} PUBLIC ${args_LIBRARIES})
    endif()

    if (args_PRIVATE_LIBRARIES)
        target_link_libraries(${NAME} PRIVATE ${args_PRIVATE_LIBRARIES})
    endif()

    if (args_PYTHON_SOURCES)
        install(FILES ${args_PYTHON_SOURCES} DESTINATION ${args_DESTINATION})
    endif()

    target_compile_definitions(
        ${NAME}
        PRIVATE
            MFB_PACKAGE_NAME=${NAME}
            MFB_ALT_PACKAGE_NAME=${NAME}
    )

    target_compile_definitions(
        ${NAME}
        PRIVATE
            BOOST_PYTHON_NO_PY_SIGNATURES
            LINUX
            PXR_PYTHON_ENABLED=1
            PXR_PYTHON_MODULES_ENABLED=1
            MFB_PACKAGE_MODULE=${NAME}
    )
endfunction() # _add_python_binding

function(_add_unit_tests) 
    set(optional ${_LIBRARY_OPTIONS})
    set(single_value ${_LIBRARY_SINGLE_VALUES})
    set(multi_value ${_LIBRARY_MULTI_VALUES})

    cmake_parse_arguments(
        PARSE_ARGV 0 args 
        "${optional}" 
        "${single_value}" 
        "${multi_value}"
    )

    foreach(NAME ${args_CLASSES})
        add_executable(${NAME} "${NAME}.cpp")

        if (args_INCLUDE_DIRS)
            target_include_directories(${NAME} PRIVATE ${args_INCLUDE_DIRS})
        endif()

        if (args_LIBRARIES)
            target_link_libraries(${NAME} PRIVATE ${args_LIBRARIES})
        endif()

        add_test(${NAME} ${NAME})
    endforeach()
    
endfunction() # _add_unit_tests
