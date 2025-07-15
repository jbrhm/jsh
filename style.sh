#! /usr/bin/env bash

set -euox pipefail

# Find clang-tidy

readonly CLANG_TIDY_PATH=$(which clang-tidy)
readonly CLANG_TIDY_ARGS=("-p=build" "-config-file=./.clang-tidy")

# Folders to analyzer

readonly FOLDERS=("./src" "./testing")

# Run clang-tidy on all src and testing
readonly TMP_ERROR="/tmp/err_out.txt"
mkdir -p /tmp
touch $TMP_ERROR 

for FOLDER in "${FOLDERS[@]}"; do
  find "${FOLDER}" -regex '.*\.\(cpp\|hpp\|h\)' | while read -r file; do
    echo "Tidying $file"
    "${CLANG_TIDY_PATH}" "${CLANG_TIDY_ARGS[@]}" "$file"
  done
done
