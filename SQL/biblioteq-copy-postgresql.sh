#!/usr/bin/env sh

# Alexis Megas.

date=$(date "+%Y%m%d")
rc=0

pg_dump -U postgres \
	--clean \
	--file=xbook_db.$date.sql xbook_db 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with pg_dump."
    exit $rc
fi

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

gzip --force --keep xbook_db.$date.sql 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with gzip."
    exit $rc
fi

chmod -rw globals.$date.sql xbook_db.$date.sql 2>/dev/null 1>&2

rc=$?

if [ ! $rc -eq 0 ]
then
    echo "Failure with chmod."
    exit $rc
fi

exit $rc
