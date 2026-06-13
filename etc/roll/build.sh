#!/bin/bash

prefix="$1"
destdir="$2"
buildflavor="$3"
numjobs="$4"

if test -n "$RP_oneTBB"; then
    tbbroot="$RP_oneTBB"
    tbbversion_header="$tbbroot/include/oneapi/tbb/version.h"
else
    tbbroot="$RP_TBB"
    tbbversion_header="$tbbroot/include/tbb/tbb_stddef.h"
fi

mkdir -p build/$buildflavor &&
cd build/$buildflavor &&
cmake  \
    -D CMAKE_INSTALL_PREFIX=$prefix \
    -D GTest_ROOT=$RP_gtest \
    -D TBB_INCLUDE_DIRS=$tbbroot/include \
    -D TBB_VERSION_H=$tbbversion_header \
    -D USD_INCLUDE_DIR=$RP_presto_usd/include \
    ../.. &&
make -j$numjobs &&
make DESTDIR=$destdir install
