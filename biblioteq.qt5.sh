#!/bin/sh

if [ -r /usr/local/biblioteq/BiblioteQ ] && [ -x /usr/local/biblioteq/BiblioteQ ]
then
    export QT_AUTO_SCREEN_SCALE_FACTOR=1

    # Disable https://en.wikipedia.org/wiki/MIT-SHM.

    export QT_X11_NO_MITSHM=1
    cd /usr/local/biblioteq && exec ./BiblioteQ
    exit $?
else
    exit 1
fi
