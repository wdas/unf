file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")

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
set(DOXYGEN_SORT_MEMBER_DOCS NO)
set(DOXYGEN_FULL_PATH_NAMES NO)
set(DOXYGEN_FILE_PATTERNS *.dox *.h)
set(DOXYGEN_PREDEFINED "PXR_NS")
set(DOXYGEN_TAGFILES
    "${PROJECT_SOURCE_DIR}/doc/doxygen/USD.tag \
    = https://graphics.pixar.com/usd/release/api/")

doxygen_add_docs(unfApiRefDoc
    "${PROJECT_SOURCE_DIR}/doc/doxygen/index.dox"
    "${PROJECT_SOURCE_DIR}/doc/doxygen/namespaces.dox"
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
