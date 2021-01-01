#!/bin/sh

# Do not remove the xbook_db.sql file after compressing it.

pg_dump -U postgres --clean --file=/disk0/biblioteq.d/xbook_db.sql xbook_db
pg_dumpall -U postgres --clean --globals-only --file=/disk0/biblioteq.d/globals.sql
gzip --force /disk0/biblioteq.d/xbook_db.sql
