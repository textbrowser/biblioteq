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
    mkdir -p Distributions/Windows/Libraries.64/postgresql
    mv pgsql/bin/libpq.dll Distributions/Windows/Libraries.64/postgresql/.
    mv 'pgsql/pgAdmin 4/runtime/libcrypto-3-x64.dll' \
       Distributions/Windows/Libraries.64/postgresql/.
    mv 'pgsql/pgAdmin 4/runtime/libiconv-2.dll' \
       Distributions/Windows/Libraries.64/postgresql/.
    mv 'pgsql/pgAdmin 4/runtime/libintl-9.dll' \
       Distributions/Windows/Libraries.64/postgresql/.
    mv 'pgsql/pgAdmin 4/runtime/libssl-3-x64.dll' \
       Distributions/Windows/Libraries.64/postgresql/.
    mv pgsql/bin/libx*.dll Distributions/Windows/Libraries.64/postgresql/.
    chmod +r,+w,-x Distributions/Windows/Libraries.64/postgresql/*.dll
    rm -f $postgresql
    rm -fr pgsql
else
    echo "Cannot read $postgresql."
fi
