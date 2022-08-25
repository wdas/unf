sphinx_add_docs(unfDoc
    SOURCE "${PROJECT_SOURCE_DIR}/doc/sphinx"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doc"
)

set(DOXYGEN_PROJECT_NAME "Usd Notice Framework")
set(DOXYGEN_HTML_OUTPUT "doc/doxygen")
set(DOXYGEN_EXTENSION_MAPPING "h=C++")
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_QUIET YES)

doxygen_add_docs(unfApiRefDoc
    "${PROJECT_SOURCE_DIR}/doc/doxygen/index.dox"
    "${PROJECT_SOURCE_DIR}/src/unf"
)

add_custom_target(documentation ALL)
add_dependencies(documentation
    unfDoc
    unfApiRefDoc
)

install(
    DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/doc"
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}
)
