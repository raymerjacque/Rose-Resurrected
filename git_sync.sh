#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

MSG="${*:-Update RosE Resurrected server codebase}"

echo "=== RosE Resurrected Git Sync ==="
git add -A

if git diff --cached --quiet; then
    echo "Working directory clean; no changes to commit."
    exit 0
fi

git commit -m "${MSG}"
git push origin main

echo "=== Successfully synchronized with GitHub! ==="
