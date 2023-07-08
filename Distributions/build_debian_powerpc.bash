#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]; then
    echo "Please install dpkg-deb."
    exit
fi

if [ ! -x /usr/bin/fakeroot ]; then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -r biblioteq.powerpc.pro ]; then
    echo "Please execute $0 from the primary directory."
    exit 1
fi

VERSION=$(grep -oP '(?<=BIBLIOTEQ_VERSION ").*(?=")' Source/biblioteq.h)

# Preparing ./opt/biblioteq:

make distclean 2>/dev/null
mkdir -p ./opt/biblioteq/Documentation
mkdir -p ./opt/biblioteq/SQL
qmake -o Makefile biblioteq.powerpc.pro
lupdate biblioteq.powerpc.pro && lrelease biblioteq.powerpc.pro
make -j $(nproc)
cp -p ./BiblioteQ ./opt/biblioteq/.
cp -p ./Icons/book.png ./opt/biblioteq/.
cp -p ./SQL/* ./opt/biblioteq/SQL/.
cp -p ./biblioteq.conf ./opt/biblioteq/.
cp -p ./biblioteq.sh ./opt/biblioteq/.
cp -pr ./Data ./opt/biblioteq/.
cp -pr ./Documentation/* ./opt/biblioteq/Documentation/.

# Preparing BiblioteQ-x.deb:

mkdir -p biblioteq-debian/opt
mkdir -p biblioteq-debian/usr/share/applications
cp -p ./biblioteq.desktop biblioteq-debian/usr/share/applications/.
cp -pr ./DEBIAN-POWERPC biblioteq-debian/DEBIAN
cp -r ./opt/biblioteq biblioteq-debian/opt/.
fakeroot dpkg-deb --build biblioteq-debian BiblioteQ-${VERSION}_powerpc.deb
make distclean
rm -fr ./opt
rm -fr biblioteq-debian
