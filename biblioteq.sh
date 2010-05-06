#!/bin/sh

ostype="`uname -s`"

if [ "$ostype" = "Darwin" ]
then
    open /Applications/BiblioteQ.d/BiblioteQ.app &
else
    cd /usr/local/biblioteq
    ./BiblioteQ &
fi

exit 0
