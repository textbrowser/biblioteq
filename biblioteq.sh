#!/usr/bin/env sh

# Alexis Megas.

set_qt_qpa_platformtheme()
{
    qt6=$(ldd "$1" 2>/dev/null | grep Qt6 2>/dev/null)

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

if [ -f ./BiblioteQ ] && \
   [ -r ./BiblioteQ ] && \
   [ -x ./BiblioteQ ]
then
    echo "Launching a local BiblioteQ."

    if [ -d ./Lib ] && [ -r ./Lib ]
    then
	export LD_LIBRARY_PATH=Lib
    fi

    set_qt_qpa_platformtheme "./BiblioteQ"
    ./BiblioteQ --configuration-file biblioteq.conf "$@"
    exit $?
fi

if [ -f /opt/biblioteq/BiblioteQ ] && \
   [ -r /opt/biblioteq/BiblioteQ ] && \
   [ -x /opt/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ (/opt/biblioteq)."
    set_qt_qpa_platformtheme "/opt/biblioteq/BiblioteQ"
    /opt/biblioteq/BiblioteQ \
	--configuration-file /opt/biblioteq/biblioteq.conf "$@"
    exit $?
fi

if [ -f /usr/local/biblioteq/BiblioteQ ] && \
   [ -r /usr/local/biblioteq/BiblioteQ ] && \
   [ -x /usr/local/biblioteq/BiblioteQ ]
then
    echo "Launching an official BiblioteQ (/usr/local/biblioteq)."
    set_qt_qpa_platformtheme "/usr/local/biblioteq/BiblioteQ"
    /usr/local/biblioteq/BiblioteQ \
	--configuration-file /usr/local/biblioteq/biblioteq.conf "$@"
    exit $?
fi

echo "BiblioteQ was not found! Please contact your lovely supervisor."
exit 1
