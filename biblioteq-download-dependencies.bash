#!/usr/bin/env bash

# Alexis Megas.

# Must be executed in the top-level source directory.

if [ "$(which wget)" = "" ]
then
    echo "Could not locate wget."
    exit 1
fi

rc=0

# PostgreSQL

postgresql="postgresql.zip"

rm -f "$postgresql"

filename="/postgresql/postgresql-16.11-2-windows-x64-binaries.zip"
host="https://get.enterprisedb.com"

wget --output-document="$postgresql" --progress=bar "$host$filename"

if [ -r "$postgresql" ]
then
    unzip -q "$postgresql"
    mkdir -p Distributions/Windows/Libraries.64
    mv pgsql/bin/libcrypto-3-x64.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/libiconv-2.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/libintl-9.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/libssl-3-x64.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/zlib1.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/libpq.dll Distributions/Windows/Libraries.64/.
    mv pgsql/bin/libx*.dll Distributions/Windows/Libraries.64/.
    chmod +r,+w,-x Distributions/Windows/Libraries.64/*.dll
    rm -f "$postgresql"
    rm -fr pgsql
else
    echo "Cannot read $postgresql."

    rc=1
fi

exit $rc
