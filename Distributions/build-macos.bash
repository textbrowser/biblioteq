#!/usr/bin/env bash
# Alexis Megas.

if [ ! -e biblioteq.macos.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

make distclean 2>/dev/null

if [ -x ~/Qt/6.8.3/macos/bin/qmake ]
then
    ~/Qt/6.8.3/macos/bin/qmake
else
    echo "Please install the official Qt."
    exit 1
fi

VERSION=$(grep -oP '(?<=BIBLIOTEQ_VERSION ").*(?=")' Source/biblioteq.h)

make -j $(sysctl -n hw.ncpu)
make install
codesign --deep --force -s "textbrowser" ./BiblioteQ.d/BiblioteQ.app
make dmg

if [ ! -r BiblioteQ.dmg ]
then
    echo "BiblioteQ.dmg is not a readable file."
    exit 1
fi

mv BiblioteQ.dmg BiblioteQ-${VERSION}_Universal.dmg
make distclean 2>/dev/null
rm -fr ./BiblioteQ.d
