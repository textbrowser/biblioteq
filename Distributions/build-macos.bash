#!/usr/bin/env bash
# Alexis Megas.

if [ ! -e biblioteq.macos.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

if [ ! -z "${SSH_TTY}" ]
then
    echo "SSH session detected. " \
	 "MacOS codesign password prompt may be invisible."
fi

make distclean 1>/dev/null 2>/dev/null

declare -a qmakes=("$HOME/Qt/6.11.1/macos/bin/qmake"
		   "$HOME/Qt/6.8.3/macos/bin/qmake")
qmake=""

for i in "${qmakes[@]}"
do
    qmake="$(echo $i)"

    if [ -x "$qmake" ]
    then
	break
    fi
done

if [ -x "$qmake" ]
then
    echo "Found $qmake."
    $qmake -o Makefile biblioteq.macos.pro 1>/dev/null 2>/dev/null
else
    echo "Cannot locate qmake. Please install the official Qt."
    exit 1
fi

VERSION="$(grep 'BIBLIOTEQ_VERSION ' Source/biblioteq.h | awk '{print $3}' | sed 's/"//g')"

echo "Making BiblioteQ."
make -j $(sysctl -n hw.ncpu) 1>/dev/null 2>/dev/null
make install 1>/dev/null 2>/dev/null
echo "Signing ./BiblioteQ.d/BiblioteQ.app."
codesign --deep --force -s "textbrowser" ./BiblioteQ.d/BiblioteQ.app \
	 1>/dev/null 2>/dev/null

if [ ! $? -eq 0 ]
then
    echo "Signing error. Bye!"
    exit 1
fi

echo "Building BiblioteQ.d.dmg."
make dmg 1>dev/null 2>/dev/null

if [ ! -r BiblioteQ.d.dmg ]
then
    echo "BiblioteQ.d.dmg is not a readable file."
    exit 1
fi

mv BiblioteQ.d.dmg BiblioteQ-${VERSION}_Universal.dmg
make distclean 1>/dev/null 2>/dev/null
rm -fr ./BiblioteQ.d
