#!/usr/bin/env bash

set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${project_dir}/build"

cmake -S "${project_dir}" -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${build_dir}" --parallel

cd "${project_dir}"

"${build_dir}/ass2" --rotation 0 --output result.png \
    --config config/config.json
"${build_dir}/ass2" --rotation 0 --output result1.png \
    --config config/config1.json
"${build_dir}/ass2" --rotation 0 --output result2.png \
    --config config/config2.json
"${build_dir}/ass2" --rotation 0 --output result3.png \
    --config config/config3.json

echo "Generated result.png, result1.png, result2.png, and result3.png in ${project_dir}"
