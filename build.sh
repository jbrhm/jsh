#!/usr/bin/env bash

set -Eeuo pipefail

if test -d build; then
    echo "Build Directory Exists..."
else
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_COMPILER=$(which clang++) \
        -DCMAKE_C_COMPILER=$(which clang) \
        -DCMAKE_CXX_FLAGS="-stdlib=libc++ -fno-rtti" \
        -DCMAKE_C_FLAGS="-stdlib=libc++" \
        . -B build -G Ninja
fi
cmake --build build
