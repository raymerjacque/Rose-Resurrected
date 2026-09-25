#!/usr/bin/env bash
set -e

DB_NAME="${1:-rose_resurrected}"
DB_USER="${2:-root}"
DB_HOST="${3:-localhost}"

echo "=== RosE Resurrected Database Importer ==="
echo "Target Database: ${DB_NAME}"
echo "User: ${DB_USER} | Host: ${DB_HOST}"

if [ -z "${DB_PASS}" ]; then
    read -s -p "Enter MySQL password [default: sproggins]: " INPUT_PASS
    echo ""
    DB_PASS="${INPUT_PASS:-sproggins}"
fi

PASS_ARG=""
if [ -n "${DB_PASS}" ]; then
    PASS_ARG="-p${DB_PASS}"
fi

# Prefer mariadb client CLI if available, fallback to mysql
DB_CLIENT="mariadb"
if ! command -v mariadb &>/dev/null; then
    DB_CLIENT="mysql"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

echo "Ensuring database '${DB_NAME}' exists..."
${DB_CLIENT} -h "${DB_HOST}" -u "${DB_USER}" ${PASS_ARG} -e "CREATE DATABASE IF NOT EXISTS \`${DB_NAME}\`;"

echo "Importing base SQL tables..."
for sqlfile in *.sql; do
    case "${sqlfile}" in
        changes.sql|Changes.sql)
            ;; # Skip changes.sql until after all tables exist
        *)
            echo "  -> ${sqlfile}"
            ${DB_CLIENT} -h "${DB_HOST}" -u "${DB_USER}" ${PASS_ARG} "${DB_NAME}" < "${sqlfile}"
            ;;
    esac
done

if [ -f "changes.sql" ]; then
    echo "Importing changes.sql..."
    ${DB_CLIENT} -h "${DB_HOST}" -u "${DB_USER}" ${PASS_ARG} "${DB_NAME}" < "changes.sql"
elif [ -f "Changes.sql" ]; then
    echo "Importing Changes.sql..."
    ${DB_CLIENT} -h "${DB_HOST}" -u "${DB_USER}" ${PASS_ARG} "${DB_NAME}" < "Changes.sql"
fi

echo "=== Database import successfully completed! ==="
