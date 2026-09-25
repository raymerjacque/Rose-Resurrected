#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

echo "=== Stopping RosE Resurrected Server Daemons ==="

for pidfile in .worldserver.pid .charserver.pid .loginserver.pid; do
    if [ -f "${pidfile}" ]; then
        PID=$(cat "${pidfile}" 2>/dev/null || true)
        if [ -n "${PID}" ] && kill -0 "${PID}" 2>/dev/null; then
            echo "Stopping PID ${PID} (${pidfile})..."
            kill "${PID}" 2>/dev/null || true
        fi
        rm -f "${pidfile}"
    fi
done

pkill -f "./loginserver" 2>/dev/null || true
pkill -f "./charserver" 2>/dev/null || true
pkill -f "./worldserver" 2>/dev/null || true

echo "=== Servers stopped ==="
