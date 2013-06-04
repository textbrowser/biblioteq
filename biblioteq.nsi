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

file *.sql
file .\qt.conf
file .\biblioteq.conf
file /r .\release\doc.d
file /r .\release\icons.d
file /r .\release\plugins

# The translations directory contains translation files
# provided by both BiblioteQ and Qt.

file /r .\release\translations.d
file .\release\*.dll
file .\libraries.win32.d\sqlite3.d\*.dll
file .\libraries.win32.d\yaz.d\*.dll
file .\libraries.win32.d\yaz.d\Microsoft.VC90.CRT.manifest
file .\release\BiblioteQ.exe

# Add an icon to the Desktop.

CreateShortCut  "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\biblioteq.exe" "" "$INSTDIR\biblioteq.exe" 0

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
