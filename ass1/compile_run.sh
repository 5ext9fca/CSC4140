#!/usr/bin/env bash

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${project_dir}/build"

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${build_dir}" --parallel

# main.cpp reads lenna.png from the working directory and writes the
# reconstructed images next to it, so run the demo from the project root.
cd "${project_dir}"
"${build_dir}/demo"

echo "Generated reconstructed_1.png, reconstructed_10.png, and reconstructed_50.png in ${project_dir}"
