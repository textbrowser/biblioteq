#!/bin/sh

if [ -r /usr/local/biblioteq/BiblioteQ ] && [ -x /usr/local/biblioteq/BiblioteQ ]
then
    cd /usr/local/biblioteq && ./BiblioteQ &
    exit $?
else
    exit 1
fi
