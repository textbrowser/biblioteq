# BiblioteQ is portable as of 2016.03.15. Please do not use this file!

# Define installer name.

Name "BiblioteQ Installer"
!define APPNAME "BiblioteQ"
outFile "BiblioteQ-Installer.exe"

# Install directory.

installDir C:\BiblioteQ

DirText "Please choose an installation directory for BiblioteQ."

# Default section start.

section

# Define output path.

setOutPath $INSTDIR

# Specify files to go in output path.

file SQL\*.sql
file .\Qt\qt.conf
file .\biblioteq.conf
file /r .\release\Documentation
file /r .\release\plugins

# The Translations directory contains translation files
# provided by both BiblioteQ and Qt.

file /r .\release\Translations
file .\release\*.dll
file .\Libraries.win32\postgresql.d\*.dll
file .\Libraries.win32\postgresql.d\Microsoft.VC90.CRT.manifest
file .\Libraries.win32\sqlite3.d\*.dll
file .\Libraries.win32\yaz.d\*.dll
file .\release\BiblioteQ.exe

# Add an icon to the Desktop.

CreateShortCut  "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\BiblioteQ.exe" "" "$INSTDIR\BiblioteQ.exe" 0

# Define uninstaller name.

writeUninstaller $INSTDIR\BiblioteQ-Uninstaller.exe

# Default section end.

sectionEnd

# Create a section to define what the uninstaller does.
# The section will always be named "Uninstall".

section "Uninstall"

Delete  "$DESKTOP\${APPNAME}.lnk"
RMDir /r $INSTDIR

sectionEnd
