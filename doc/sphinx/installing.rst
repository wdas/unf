.. _installing:

**********
Installing
**********

.. highlight:: bash

The library can be installed using :term:`CMake` (any version over `3.15
<https://cmake.org/cmake/help/latest/release/3.15.html>`_).

.. _installing/dependencies:

Dependencies
============

Ensure that a minimal installation of :term:`USD` is available. Headers and
compiled libraries from :term:`USD` as well as headers from transitive
dependencies such as :term:`TBB` are required during the building process.

If Python bindings are needed, :term:`USD` must be built with Python support.
Compiled libraries and headers from Python are also required during the
building process.

.. seealso::

    `Building USD
    <https://github.com/PixarAnimationStudios/OpenUSD/blob/release/BUILDING.md>`_


Custom search paths to dependent packages can be provided with the following
:term:`CMake` options (or environment variable):

============================= =========================================================
Option / Environment Variable Description
============================= =========================================================
USD_ROOT                      Add search path to :term:`USD` package.
TBB_ROOT                      Add search path to :term:`TBB` package.
Python_ROOT                   Add search path to Python package.
Pytest_ROOT                   Add search path to :term:`pytest` program.
Doxygen_ROOT                  Add search path to :term:`doxygen` program.
Sphinx_ROOT                   Add search path to :term:`sphinx-build <Sphinx>` program.
ClangFormat_ROOT              Add search path to :term:`clang-format` program.
============================= =========================================================

.. note::

    These feature is provided by :term:`CMake` under the `CMP0074
    <https://cmake.org/cmake/help/latest/policy/CMP0074.html>`_ policy

.. _installing/building:

Building library
================

Obtain a copy of the source by either downloading the
`zipball <https://github.com/wdas/unf/archive/main.zip>`_ or
cloning the public repository::

    git clone git@github.com:wdas/unf.git

Then you can build and install the library as follows::

    cd unf
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=/path/to/destination ..
    cmake --build . --target install

Here are a few :term:`CMake` options that can be used to influence the building
process:

===================== ==================================================================
Option                Description
===================== ==================================================================
BUILD_TESTS           Indicate whether tests should be built. Default is true.
BUILD_DOCS            Indicate whether documentation should be built. Default is true.
BUILD_PYTHON_BINDINGS Indicate whether Python bindings should be built. Default is true.
BUILD_SHARED_LIBS     Indicate whether library should be built shared. Default is true.
BUNDLE_PYTHON_TESTS   Bundle Python tests per group (faster). Default is false.
===================== ==================================================================

The library can then be used by other programs or libraries via the ``unf::unf``
:term:`Cmake` target.

.. _installing/clang-format:

Apply clang-format
==================

Ensure that :term:`Clang-format` is installed for applying C++ style.

Then run the program as follows::

    cmake --build . --target format

.. warning::

    When contributing, please run this command before committing your code.

.. _installing/documentation:

Building documentation
======================

Ensure that :term:`Doxygen` is installed. The required Python dependencies
must also be installed as follows::

    pip install -r doc/requirements.txt

Then build the documentation as follows::

    cmake --build . --target documentation

.. note::

    Documentation is automatically built with default installation, unless you
    set the ``BUILD_DOCS`` :term:`CMake` option to false.

.. _installing/test:

Running tests
=============

Ensure that :term:`GTest` is installed. The required Python dependencies
must also be installed as follows::

    pip install -r test/requirements.txt

.. note::

    Python dependencies are not necessary if the ``BUILD_PYTHON_BINDINGS``
    :term:`CMake` option is set to false.

Once the library and all tests are built, you can run the tests using
:term:`Ctest` within the build folder as follows::

    ctest

You can increase the verbosity and filter in one or several tests as follows::

    ctest -VV
    ctest -R BrokerTest.Create -VV
    ctest -R BrokerTest.* -VV

.. note::

    Tests are automatically built with default installation, unless you
    set the ``BUILD_TESTS`` :term:`CMake` option to false.

By default, unit tests and integration tests in Python will be decomposed into
separated tests that can be individually filtered. Set the
``BUNDLE_PYTHON_TESTS`` :term:`CMake` option (or environment variable) to true
if you want to combine Python tests per test type.
