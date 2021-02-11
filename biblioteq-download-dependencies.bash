#!/bin/bash

# Must be executed in the top-level source directory.

# Poppler

brotli=mingw-w64-i686-brotli-1.0.9-2-any.pkg.tar.zst
curl=mingw-w64-i686-curl-7.74.0-2-any.pkg.tar.zst
idn2=mingw-w64-i686-libidn2-2.3.0-1-any.pkg.tar.xz
poppler=mingw-w64-i686-poppler-20.12.1-2-any.pkg.tar.zst

rm -f $brotli
rm -f $curl
rm -f $poppler
wget --output-document=$brotli \
     --progress=bar \
     "https://repo.msys2.org/mingw/i686/$brotli"
wget --output-document=$curl \
     --progress=bar \
     "https://repo.msys2.org/mingw/i686/$curl"
wget --output-document=$idn2 \
     --progress=bar \
     "https://repo.msys2.org/mingw/i686/$idn2"
wget --output-document=$poppler \
     --progress=bar \
     "https://repo.msys2.org/mingw/i686/$poppler"

if [ -r $brotli ] && \
       [ -r $curl ] && \
       [ -r $idn2 ] && \
       [ -r $poppler ]; then
    tar -I zstd -vxf $brotli
    cp mingw32/bin/*.dll Libraries.win32/poppler/.
    rm -rf mingw32
    tar -I zstd -vxf $curl
    cp mingw32/bin/libcurl-4.dll Libraries.win32/poppler/.
    rm -fr mingw32
    tar -I zstd -vxf $idn2
    cp mingw32/bin/*.dll Libraries.win32/poppler/.
    rm -fr mingw32
    tar -I zstd -vxf $poppler
    cp mingw32/bin/libpoppler-105.dll Libraries.win32/poppler/.
    cp mingw32/bin/libpoppler-qt5-1.dll Libraries.win32/poppler/.
    cp mingw32/include/poppler/cpp/* Include.win32/poppler/cpp/.
    cp mingw32/include/poppler/qt5/* Include.win32/poppler/qt5/.
    chmod +w Include.win32/poppler/cpp/*.h
    chmod +w Include.win32/poppler/qt5/*.h
    chmod +w,-x Libraries.win32/poppler/*.dll
    rm -fr .BUILDINFO .MTREE .PKGINFO mingw32
    rm -f $brotli $curl $idn2 $poppler
else
    echo "Cannot process Poppler."
fi

exit 0

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

sqlite=sqlite-dll-win32-x86-3340100.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2021/$sqlite

if [ -r $sqlite ]; then
    unzip -q -o $sqlite
    mv sqlite3.def sqlite3.dll Libraries.win32/sqlite3/.
    chmod +r,+w,-x Libraries.win32/sqlite3/*.dll
    rm -f $sqlite
else
    echo "Cannot read $sqlite."
fi

# SQLite Source

sqlite=sqlite-amalgamation-3340100.zip

rm -f $sqlite
wget --progress=bar https://sqlite.org/2021/$sqlite

if [ -r $sqlite ]; then
    unzip -q -o $sqlite
    rm -f $sqlite
else
    echo "Cannot read $sqlite."
fi

sqlite=sqlite-amalgamation-3340100

if [ -r $sqlite ]; then
    mv $sqlite/*.h Include.win32/sqlite3/.
    rm -fr $sqlite
else
    echo "Cannot read $sqlite."
fi
