#!/usr/bin/env bash
# Alexis Megas.

if [ ! -r biblioteq.pro ]; then
    echo "Please execute $0 from the primary directory."
    exit 1
fi

# Prepare ./biblioteq.

make distclean 2>/dev/null
mkdir -p ./biblioteq/Documentation
mkdir -p ./biblioteq/Lib
mkdir -p ./biblioteq/SQL
qmake -o Makefile biblioteq.pro
lupdate biblioteq.pro && lrelease biblioteq.pro
make -j $(nproc)
cp -p ./BiblioteQ ./biblioteq/.
cp -p ./Icons/book.png ./biblioteq/.
cp -p ./SQL/* ./biblioteq/SQL/.
cp -p ./biblioteq.conf ./biblioteq/.
cp -p ./biblioteq.sh ./biblioteq/.
cp -pr ./Data ./biblioteq/.
cp -pr ./Documentation/* ./biblioteq/Documentation/.

# Prepare a tar bundle.

cp $(ldd ./BiblioteQ | awk '{print $3}' | grep -e '^/') ./biblioteq/Lib/.
chmod -x ./biblioteq/Lib/*.so*
tar -cvz -f BiblioteQ-2023.05.20.tar.gz ./biblioteq
make distclean
rm -fr ./biblioteq
