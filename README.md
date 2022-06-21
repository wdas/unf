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

Here a few options that can be used:

| Option                 | Description                                                         |
| ---------------------- | ------------------------------------------------------------------- |
| PXR_USD_LOCATION       | Define path to USD location to target.                              |
| BUILD_TESTS            | Indicate whether tests should be built. Default is true.            |
| BUILD_PYTHON_BINDINGS  | Indicate whether Python bindings should be built. Default is true.  |
| BUILD_SHARED_LIBS      | Indicate whether library should be built shared. Default is true.   |

## Testing

Once the library and all tests are build, run the tests as follows:

```bash
ctest -VV
```
