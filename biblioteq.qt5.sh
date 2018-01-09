#!/bin/sh

if [ -r /usr/local/biblioteq/BiblioteQ ] && [ -x /usr/local/biblioteq/BiblioteQ ]
then
    export LD_LIBRARY_PATH=/usr/local/biblioteq/Lib
    cd /usr/local/biblioteq && exec ./BiblioteQ -style fusion
    exit $?
else
    exit 1
fi
