#!/usr/bin/env sh
# Alexis Megas.

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1
kde=$(env | grep -ci kde 2>/dev/null)

if [ $kde -gt 0 ]
then
    echo "KDE!"
    style="-style=Breeze"
else
    style="-style=Fusion"
fi

if [ -r ./BiblioteQ ] && [ -x ./BiblioteQ ]
then
    echo "Launching a local BiblioteQ."

    if [ -r ./Lib ]
    then
	export LD_LIBRARY_PATH=Lib
    fi

    exec ./BiblioteQ "$style" "$@"
    exit $?
fi

if [ -r /opt/biblioteq/BiblioteQ ] && [ -x /opt/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ."
    cd /opt/biblioteq && exec ./BiblioteQ "$style" "$@"
    exit $?
fi

if [ -r /usr/local/biblioteq/BiblioteQ ] &&
   [ -x /usr/local/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ."
    cd /usr/local/biblioteq && exec ./BiblioteQ "$style" "$@"
    exit $?
fi

echo "BiblioteQ was not found!"
exit 1
