#!/bin/sh

if [ -x /usr/local/biblioteq/BiblioteQ ]
then
    cd /usr/local/biblioteq
    ./BiblioteQ &
else
    exit 1
fi

exit 0
