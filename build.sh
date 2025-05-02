#!/usr/bin/env bash

set -Eeuo pipefail

if test -d build; then
    echo "Build Directory Exists..."
else
    cmake \
        -DCMAKE_CXX_COMPILER=$(which g++) \
        -DCMAKE_C_COMPILER=$(which gcc) \
        . -B build -G Ninja
fi
cmake --build build
