#!/bin/bash

# Must be executed in the top-level source directory.

# PostgreSQL

postgresql=postgresql.zip

rm -f $postgresql
wget --output-document=$postgresql \
     --progress=bar \
     "https://get.enterprisedb.com/postgresql/postgresql-9.6.16-1-windows-binaries.zip"
unzip $postgresql
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

# SQLite Binaries

sqlite=sqlite-dll-win32-x86-3300100.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2019/$sqlite
unzip -o $sqlite
mv sqlite3.def sqlite3.dll Libraries.win32/sqlite3/.
chmod +r,+w,-x Libraries.win32/sqlite3/*.dll
rm -f $sqlite

# SQLite Source

sqlite=sqlite-amalgamation-3300100.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2019/$sqlite
unzip -o $sqlite
rm -f $sqlite

sqlite=sqlite-amalgamation-3300100

mv $sqlite/*.h Include.win32/sqlite3/.
rm -fr $sqlite
