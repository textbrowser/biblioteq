#!/bin/sh

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -r ./BiblioteQ ] && [ -x ./BiblioteQ ]
then
    echo "Launching a local BiblioteQ."
    exec ./BiblioteQ "$@"
    exit $?
fi

if [ -r /usr/local/biblioteq/BiblioteQ ] &&
   [ -x /usr/local/biblioteq/BiblioteQ ]
then
    cd /usr/local/biblioteq && exec ./BiblioteQ "$@"
    exit $?
else
    exit 1
fi
