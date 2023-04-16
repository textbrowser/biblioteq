#!/usr/bin/env sh
# Alexis Megas.

date=$(date "+%Y%m%d")

pg_dump -U postgres --clean --file=xbook_db.$date.sql xbook_db
pg_dumpall -U postgres --clean --globals-only --file=globals.$date.sql
gzip --force --keep xbook_db.$date.sql
chmod -rw globals.$date.sql xbook_db.$date.sql
