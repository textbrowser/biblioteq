#!/usr/bin/env sh

# Alexis Megas.

date=$(date "+%Y%m%d")
rc=0

echo "Exporting the PostgreQL database xbook_db via pg_dump."
pg_dump -U postgres \
	--clean \
	--file=xbook_db.$date.sql xbook_db 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with pg_dump."
    exit $rc
fi

echo "Exporting global objects (roles and tables) of xbook_db via pg_dumpall."
pg_dumpall -U postgres \
	   --clean \
	   --globals-only \
	   --file=globals.$date.sql 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with pg_dumpall."
    exit $rc
fi

echo "Compressing xbook_db.$date.sql via gzip."
gzip --force --keep xbook_db.$date.sql 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with gzip."
    exit $rc
fi

echo "Setting permissions on globals.$date.sql, xbook_db.$date.sql."
chmod -rw globals.$date.sql xbook_db.$date.sql 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with chmod."
    exit $rc
fi

exit $rc
