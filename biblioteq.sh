#!/usr/bin/env sh

# Alexis Megas.

set_qt_qpa_platformtheme()
{
    qt6=$(ldd "$1" | grep Qt6 2>/dev/null)

    if [ ! -z "$qt6" ]
    then
	if [ ! -z "$(which qt6ct)" ]
	then
	    echo "Exporting QT_QPA_PLATFORMTHEME as qt6ct."

	    export QT_QPA_PLATFORMTHEME=qt6ct
	fi
    fi
}

export QT_AUTO_SCREEN_SCALE_FACTOR=1

# Disable https://en.wikipedia.org/wiki/MIT-SHM.

export QT_X11_NO_MITSHM=1

if [ -r ./BiblioteQ ] && [ -x ./BiblioteQ ]
then
    echo "Launching a local BiblioteQ."

    if [ -r ./Lib ]
    then
	export LD_LIBRARY_PATH=Lib
    fi

    set_qt_qpa_platformtheme "./BiblioteQ"
    ./BiblioteQ --configuration-file biblioteq.conf "$@"
    exit $?
fi

if [ -r /opt/biblioteq/BiblioteQ ] && [ -x /opt/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ."
    set_qt_qpa_platformtheme "/opt/biblioteq/BiblioteQ"
    /opt/biblioteq/BiblioteQ \
	--configuration-file /opt/biblioteq/biblioteq.conf "$@"
    exit $?
fi

if [ -r /usr/local/biblioteq/BiblioteQ ] &&
   [ -x /usr/local/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ."
    set_qt_qpa_platformtheme "/usr/local/biblioteq/BiblioteQ"
    /usr/local/biblioteq/BiblioteQ \
	--configuration-file /usr/local/biblioteq/biblioteq.conf "$@"
    exit $?
fi

echo "BiblioteQ was not found! Please contact your lovely supervisor."
exit 1
