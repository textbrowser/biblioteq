#!/bin/bash

# Must be executed in the top-level source directory.

# PostgreSQL

postgresql=postgresql.zip

rm -f $postgresql
wget --output-document=$postgresql \
     --progress=bar \
     "https://get.enterprisedb.com/postgresql/postgresql-9.6.20-1-windows-binaries.zip"

if [ -r $postgresql ]; then
    unzip -q $postgresql
    mv pgsql/bin/libcrypto-1_1.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libiconv-2.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libintl-8.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libpq.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libssl-1_1.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libxml2.dll Libraries.win32/postgresql/.
    mv pgsql/bin/libxslt.dll Libraries.win32/postgresql/.
    chmod +r,+w,-x Libraries.win32/postgresql/*
    rm -f $postgresql
    rm -fr pgsql
else
    echo "Cannot read $postgresql."
fi

# SQLite Binaries

sqlite=sqlite-dll-win32-x86-3340000.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2020/$sqlite

if [ -r $sqlite ]; then
    unzip -q -o $sqlite
    mv sqlite3.def sqlite3.dll Libraries.win32/sqlite3/.
    chmod +r,+w,-x Libraries.win32/sqlite3/*.dll
    rm -f $sqlite
else
    echo "Cannot read $sqlite."
fi

# SQLite Source

sqlite=sqlite-amalgamation-3340000.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2020/$sqlite

if [ -r $sqlite ]; then
    unzip -q -o $sqlite
    rm -f $sqlite
else
    echo "Cannot read $sqlite."
fi

sqlite=sqlite-amalgamation-3340000

if [ -r $sqlite ]; then
    mv $sqlite/*.h Include.win32/sqlite3/.
    rm -fr $sqlite
else
    echo "Cannot read $sqlite."
fi
