# USD Notice Broker

[![Tests](https://github.com/wdas/usd-notice-broker/actions/workflows/test.yml/badge.svg?branch=prototype)](https://github.com/wdas/usd-notice-broker/actions/workflows/test.yml)

Notice management library built over USD Notices

## Building

Build the library as follows:

```bash
cd usd-notice-broker
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/tmp ..
make & make install
```

A specific USD location can be targeted as follows:

```bash
cmake -DPXR_USD_LOCATION=/path/to/usd -DCMAKE_INSTALL_PREFIX=/tmp ..
```

## Testing

Once the library and all tests are build, run the tests as follows:

```bash
make test
```
