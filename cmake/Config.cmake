include(Macros)
include(GNUInstallDirs)

option(BUILD_TESTS "Build tests" ON)

# Set up installation sub-folders.
set(INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(INSTALL_PYTHONDIR "${INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages")
