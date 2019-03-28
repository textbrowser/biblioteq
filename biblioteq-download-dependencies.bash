#!/bin/bash

# Must be executed in the top-level source directory.

# PostgreSQL

postgresql=postgresql.zip

rm -f $postgresql
wget --output-document=$postgresql \
     --progress=bar \
     "https://get.enterprisedb.com/postgresql/postgresql-9.6.12-1-windows-binaries.zip?ls=Crossover&type=Crossover&_ga=2.53582379.223986836.1553804077-504097994.1552479753"

unzip $postgresql
mv pgsql/bin/libeay32.dll Libraries.win32/postgresql.d/.
mv pgsql/bin/libiconv-2.dll Libraries.win32/postgresql.d/.
mv pgsql/bin/libintl-8.dll Libraries.win32/postgresql.d/.
mv pgsql/bin/libpq.dll Libraries.win32/postgresql.d/.
mv pgsql/bin/libxml2.dll Libraries.win32/postgresql.d/.
mv pgsql/bin/libxslt.dll Libraries.win32/postgresql.d/.
chmod +r,+w,-x Libraries.win32/postgresql.d/*
rm -f $postgresql
rm -fr pgsql

# SQLite Binaries

sqlite=sqlite-dll-win32-x86-3270200.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2019/$sqlite
unzip -o $sqlite
mv sqlite3.def sqlite3.dll Libraries.win32/sqlite3.d/.
chmod +r,+w,-x Libraries.win32/sqlite3.d/*.dll
rm -f $sqlite

# SQLite Source

sqlite=sqlite-amalgamation-3270200.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2019/$sqlite
unzip -o $sqlite
rm -f $sqlite

sqlite=sqlite-amalgamation-3270200

mv $sqlite/*.h Include.win32/sqlite3/.
rm -fr $sqlite
