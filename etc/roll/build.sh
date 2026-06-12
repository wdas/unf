#!/bin/bash

prefix="$1"
destdir="$2"
buildflavor="$3"
numjobs="$4"

mkdir -p build/$buildflavor &&
cd build/$buildflavor &&
cmake  \
    -D CMAKE_INSTALL_PREFIX=$prefix \
    -D GTest_ROOT=$RP_gtest \
    -D TBB_INCLUDE_DIRS=$RP_TBB/include \
    -D USD_INCLUDE_DIR=$RP_presto_usd/include \
    ../.. &&
make -j$numjobs &&
make DESTDIR=$destdir install
