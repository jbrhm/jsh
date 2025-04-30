#!/usr/bin/env bash

set -Eeuo pipefail

if test -d build; then
    echo "Build Directory Exists..."
else
    cmake . -B build -G Ninja
fi
cmake --build build
