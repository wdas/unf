# USD Notice Framework

[![Tests](https://github.com/wdas/usd-notice-framework/actions/workflows/test.yml/badge.svg?branch=main)](https://github.com/wdas/usd-notice-framework/actions/workflows/test.yml)

Notice management library built over USD Notices.

## Building

Build the library as follows:

```bash
cd usd-notice-framework
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp ..
cmake --build . --target install
```

Here a few CMake options that can be used to influence the building process:

| Option                 | Description                                                         |
| ---------------------- | ------------------------------------------------------------------- |
| BUILD_TESTS            | Indicate whether tests should be built. Default is true.            |
| BUILD_DOCS             | Indicate whether documentation should be built. Default is true.    |
| BUILD_PYTHON_BINDINGS  | Indicate whether Python bindings should be built. Default is true.  |
| BUILD_SHARED_LIBS      | Indicate whether library should be built shared. Default is true.   |
| BUNDLE_PYTHON_TESTS    | Bundle Python tests per group (faster). Default is false.           |

Custom search paths to dependent packages can be provided with the following
Cmake options (or environment variable):

| Option / Environment Variable  | Description                              |
| ------------------------------ | ---------------------------------------- |
| USD_ROOT                       | Add search path to USD package.          |
| TBB_ROOT                       | Add search path to TBB package.          |
| Boost_ROOT                     | Add search path to Boost package.        |
| Pytest_ROOT                    | Add search path to pytest program.       |
| Sphinx_ROOT                    | Add search path to sphinx-build program. |
| ClangFormat_ROOT               | Add search path to clang-format program. |

For instance:

```bash
cmake -DUSD_ROOT=/path/to/usd -DCMAKE_INSTALL_PREFIX=/tmp ..
```

The Python library can also be built and installed locally using
[pip](https://pip.pypa.io/en/stable/) and
[scikit-build](https://scikit-build.readthedocs.io/en/stable/):

```bash
pip install .
```

NOTE: The Python distribution workflow is not clearly defined yet.

## Testing

Once the library and all tests are build (with the `BUILD_TESTS` option), run
the tests as follows:

```bash
ctest -VV
```

Ensure that [pytest](https://docs.pytest.org/en/stable/) is installed to test
python bindings.
