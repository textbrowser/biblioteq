#!/bin/sh

date=$(date "+%Y%m%d")

pg_dump -U postgres \
	--clean --file=/disk0/biblioteq.d/xbook_db.$date.sql xbook_db
pg_dumpall -U postgres \
	   --clean --globals-only --file=/disk0/biblioteq.d/globals.$date.sql
gzip --force --keep /disk0/biblioteq.d/xbook_db.$date.sql
