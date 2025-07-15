#! /usr/bin/env bash

set -euox pipefail

# Find clang-tidy and clang-format

readonly CLANG_TIDY_PATH=$(which clang-tidy)
readonly CLANG_TIDY_ARGS=("-p=build" "-config-file=./.clang-tidy")
readonly CLANG_FORMAT_PATH=$(which clang-format)
CLANG_FORMAT_ARGS=("-style=file")
if [ $# -eq 0 ] || [ "$1" != "--fix" ]; then
  CLANG_FORMAT_ARGS=("--dry-run" "--Werror")
fi

# Folders to analyze

readonly FOLDERS=("./src" "./testing")

# Run clang-tidy and clang-format on all src and testing
readonly TMP_ERROR="/tmp/err_out.txt"
mkdir -p /tmp
touch $TMP_ERROR 

for FOLDER in "${FOLDERS[@]}"; do
  find "${FOLDER}" -regex '.*\.\(cpp\|hpp\|h\)' | while read -r file; do
    echo "Styling $file"
    "${CLANG_TIDY_PATH}" "${CLANG_TIDY_ARGS[@]}" "$file"
    "${CLANG_FORMAT_PATH}" "${CLANG_FORMAT_ARGS[@]}" -i "$file"
  done
done
