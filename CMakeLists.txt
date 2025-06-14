if(WIN32)
    cmake_minimum_required(VERSION 3.27)
else()
    cmake_minimum_required(VERSION 3.20)
endif()

project(unf
    VERSION 0.8.3
    LANGUAGES CXX
)

include(GNUInstallDirs)

if (DEFINED ENV{CXXFLAGS_STD})
    string(SUBSTRING "$ENV{CXXFLAGS_STD}" 3 -1 cxx_std)
else()
    set(cxx_std 17)
endif()
set(CMAKE_CXX_STANDARD "${cxx_std}" CACHE STRING "Default C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # Make sure WinDef.h does not define min and max macros which
    # will conflict with std::min() and std::max().
    add_compile_definitions("NOMINMAX")

    # From OpenUSD/cmake/defaults/msvcdefaults.cmake
    #
    # The /Zc:inline option strips out the "arch_ctor_<name>" symbols used for
    # library initialization by ARCH_CONSTRUCTOR starting in Visual Studio 2019,
    # causing release builds to fail. Disable the option for this and later
    # versions.
    #
    # For more details, see:
    # https://developercommunity.visualstudio.com/content/problem/914943/zcinline-removes-extern-symbols-inside-anonymous-n.html
    if (MSVC_VERSION GREATER_EQUAL 1920)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:inline-")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:inline")
    endif()
endif()

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" ON)
option(BUILD_PYTHON_BINDINGS "Build Python Bindings" ON)
option(BUNDLE_PYTHON_TESTS "Bundle Python tests per group (faster)" OFF)
option(BUILD_SHARED_LIBS "Build Shared Library" ON)

# Update build type from environment for CMake < 3.22
if (DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE}
        CACHE STRING "Specifies the build type" FORCE)
endif()

# Make module finder scripts available for TBB and USD.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# Silence "Up-to-date:" install messages
set(CMAKE_INSTALL_MESSAGE NEVER)

# Generate "compile_commands.json" for use by editors and other developer tools.
# https://cmake.org/cmake/help/v3.5/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Discover Python per best matching location instead of highest version.
# https://cmake.org/cmake/help/latest/policy/CMP0094.html
cmake_policy(SET CMP0094 NEW)

find_package(USD 0.20.11 REQUIRED)
find_package(TBB 2017.0 COMPONENTS tbb REQUIRED)

if(BUILD_PYTHON_BINDINGS)
    # The 'manylinux' images do not include the Python library.
    # CMake >= 3.18 is required for this option to work as expected.
    # https://github.com/pypa/manylinux
    if (TARGET_PYTHON_MODULE OR ($ENV{TARGET_PYTHON_MODULE}))
        find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
        add_library(Python::Python ALIAS Python::Module)
    else()
        find_package(Python COMPONENTS Interpreter Development REQUIRED)
    endif()

    # Convenient variable to fetch module against Python version found.
    set(_py_version ${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
    mark_as_advanced(_py_version)

    if (NOT USD_USE_INTERNAL_BOOST_PYTHON)
        find_package(Boost 1.70.0 COMPONENTS "python${_py_version}" REQUIRED)

        # Define generic target for Boost Python if necessary.
        if (NOT TARGET Boost::python)
            add_library(Boost::python ALIAS "Boost::python${_py_version}")
        endif()
    endif()

    # Set variable to identify the path to install and test python libraries.
    set(PYTHON_VERSION
        "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}"
        CACHE INTERNAL "Python version.")

    set(PYTHON_DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION}/site-packages"
        CACHE INTERNAL "Python library path.")
endif()

add_subdirectory(src)

if (BUILD_TESTS)
    find_package(GTest 1.8.0 REQUIRED)
    include(GoogleTest)

    if(BUILD_PYTHON_BINDINGS)
        find_package(Pytest 4.6.11 REQUIRED)
    endif()

    enable_testing()

    add_subdirectory(test)
endif()

if (BUILD_DOCS)
    find_package(Sphinx 1.8.6 REQUIRED)
    find_package(Doxygen 1.8.5 REQUIRED)

    add_subdirectory(doc)
endif()

# Add format target if clang-format is found.
find_package(ClangFormat 7)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/unf-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/unf-config.cmake"
    INSTALL_DESTINATION share/cmake/unf
)

write_basic_package_version_file(
    "unf-config-version.cmake"
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/unf-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/unf-config-version.cmake"
    DESTINATION share/cmake/unf
)
