#!/bin/sh

if [ -r /usr/local/biblioteq/BiblioteQ ] && [ -x /usr/local/biblioteq/BiblioteQ ]
then
    export LD_LIBRARY_PATH=/usr/local/biblioteq/Lib
    # Disable https://en.wikipedia.org/wiki/MIT-SHM.
    export QT_X11_NO_MITSHM=1
    cd /usr/local/biblioteq && exec ./BiblioteQ -style fusion
    exit $?
else
    exit 1
fi
