include(Macros)
include(GNUInstallDirs)

option(BUILD_TESTS "Build tests" ON)

# Silence deprecation warnings related to TF's use of hash map.
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-deprecated")

# Python bindings for TF require this define.
add_definitions("-DBOOST_PYTHON_NO_PY_SIGNATURES")

# Suppress deprecation messages from TBB.
add_definitions("-DTBB_SUPPRESS_DEPRECATED_MESSAGES")

# Workaround for an issue in which Python headers unequivocally
# redefine macros defined in standard library headers.
# https://github.com/PixarAnimationStudios/USD/issues/1
include_directories(SYSTEM ${Python_INCLUDE_DIR})

# Extract Major and Minor version of Python.
set(PYTHON_VERSION "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")

# Set up installation sub-folders.
set(INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(INSTALL_PYTHONDIR "${INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages")
