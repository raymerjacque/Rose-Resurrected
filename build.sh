#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p "${SCRIPT_DIR}/build"
cd "${SCRIPT_DIR}/build"

echo "=== Configuring RosE Resurrected Server (CMake) ==="
cmake ..

echo "=== Building Servers ==="
make -j"$(nproc)"

echo "=== Build Complete! ==="
echo "Binaries are located in ${SCRIPT_DIR}/VS_Projects/Server.Run.2/"
