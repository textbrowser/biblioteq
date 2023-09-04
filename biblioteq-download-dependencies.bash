#!/usr/bin/env bash
# Alexis Megas.

# Must be executed in the top-level source directory.

# PostgreSQL

postgresql=postgresql.zip

rm -f $postgresql
wget --output-document=$postgresql \
     --progress=bar \
     "https://get.enterprisedb.com/postgresql/postgresql-15.4-1-windows-x64-binaries.zip"

if [ -r $postgresql ]; then
    unzip -q $postgresql
    mkdir -p Libraries.win64/postgresql
    mv pgsql/lib/libpq.dll Libraries.win64/postgresql/.
    mv 'pgsql/pgAdmin 4/bin/libiconv-2.dll' Libraries.win64/postgresql/.
    mv 'pgsql/pgAdmin 4/bin/libintl-8.dll' Libraries.win64/postgresql/.
    mv 'pgsql/pgAdmin 4/venv/DLLs/libcrypto-1_1.dll' \
       Libraries.win64/postgresql/.
    mv 'pgsql/pgAdmin 4/venv/DLLs/libssl-1_1.dll' Libraries.win64/postgresql/.
    mv pgsql/bin/libx*.dll Libraries.win64/postgresql/.
    chmod +r,+w,-x Libraries.win64/postgresql/*
    rm -f $postgresql
    rm -fr pgsql
else
    echo "Cannot read $postgresql."
fi
