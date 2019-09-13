#!/bin/sh

if [ -r /usr/local/biblioteq/BiblioteQ ] && [ -x /usr/local/biblioteq/BiblioteQ ]
then
    # Disable https://en.wikipedia.org/wiki/MIT-SHM.
    export QT_X11_NO_MITSHM=1
    cd /usr/local/biblioteq && exec ./BiblioteQ -style Fusion
    exit $?
else
    exit 1
fi
