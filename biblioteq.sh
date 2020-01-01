#!/bin/sh

if [ -r /usr/local/biblioteq-qt4/BiblioteQ ] && [ -x /usr/local/biblioteq-qt4/BiblioteQ ]
then
    # Disable https://en.wikipedia.org/wiki/MIT-SHM.

    export QT_X11_NO_MITSHM=1
    cd /usr/local/biblioteq-qt4 && exec ./BiblioteQ
    exit $?
else
    exit 1
fi
