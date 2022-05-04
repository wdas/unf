include(Macros)
include(GNUInstallDirs)

option(BUILD_TESTS "Build tests" ON)

# Extract Major and Minor version of Python.
string(REGEX REPLACE "\.[1-9]+$" "" PYTHON_VERSION ${PYTHONLIBS_VERSION_STRING})

# Silence deprecation warnings related to TF's use of hash map.
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-deprecated")

# Python bindings for TF require this define.
add_definitions("-DBOOST_PYTHON_NO_PY_SIGNATURES")

# Suppress deprecation messages from TBB.
add_definitions("-DTBB_SUPPRESS_DEPRECATED_MESSAGES")

# Workaround for an issue in which Python headers unequivocally
# redefine macros defined in standard library headers.
# https://github.com/PixarAnimationStudios/USD/issues/1
include_directories(SYSTEM ${PYTHON_INCLUDE_DIR})

# Set up installation sub-folders.
set(INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(INSTALL_PYTHONDIR "${INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages")
