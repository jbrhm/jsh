#!/usr/bin/env bash

set -Eeuo pipefail

if test -d build; then
    echo "Build Directory Exists..."
else
    cmake \
        -DCMAKE_CXX_COMPILER=$(which clang++) \
        -DCMAKE_C_COMPILER=$(which clang) \
        . -B build -G Ninja
fi
cmake --build build
