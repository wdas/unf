# USD Notice Broker

[![Tests](https://github.com/wdas/usd-notice-broker/actions/workflows/test.yml/badge.svg?branch=prototype)](https://github.com/wdas/usd-notice-broker/actions/workflows/test.yml)

Notice management library built over USD Notices.

## Building

Build the library as follows:

```bash
cd usd-notice-broker
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp ..
cmake --build . --target install
```

Here a few CMake options that can be used to influence the building process:

| Option                 | Description                                                         |
| ---------------------- | ------------------------------------------------------------------- |
| BUILD_TESTS            | Indicate whether tests should be built. Default is true.            |
| BUILD_PYTHON_BINDINGS  | Indicate whether Python bindings should be built. Default is true.  |
| BUILD_SHARED_LIBS      | Indicate whether library should be built shared. Default is true.   |

Custom search paths to dependent packages can be provided with the following
Cmake options (or environment variable):

| Option / ENV    | Description                         |
| --------------- | ----------------------------------- |
| USD_ROOT        | Add search path to USD location.    |
| TBB_ROOT        | Add search path to TBB location.    |
| BOOST_ROOT      | Add search path to Boost location.  |

For instance:

```bash
cmake -DUSD_ROOT=/path/to/usd -DCMAKE_INSTALL_PREFIX=/tmp ..
```

## Testing

Once the library and all tests are build, run the tests as follows:

```bash
ctest -VV
```

