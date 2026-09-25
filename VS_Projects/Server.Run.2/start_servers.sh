#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

mkdir -p log plog

echo "=== Starting RosE Resurrected Server Daemons ==="

echo "Starting Login Server..."
nohup ./loginserver </dev/null > log/loginserver_stdout.log 2>&1 &
LOGIN_PID=$!
echo "  Login Server PID: ${LOGIN_PID}"

sleep 2

echo "Starting Character Server..."
nohup ./charserver </dev/null > log/charserver_stdout.log 2>&1 &
CHAR_PID=$!
echo "  Character Server PID: ${CHAR_PID}"

sleep 2

echo "Starting World Server..."
nohup ./worldserver </dev/null > log/worldserver_stdout.log 2>&1 &
WORLD_PID=$!
echo "  World Server PID: ${WORLD_PID}"

echo "${LOGIN_PID}" > .loginserver.pid
echo "${CHAR_PID}" > .charserver.pid
echo "${WORLD_PID}" > .worldserver.pid
disown -a

echo "=== All servers launched ==="
echo "Logs are available in: ${SCRIPT_DIR}/log/"
