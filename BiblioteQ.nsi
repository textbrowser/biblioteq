# Define installer name.
Name "BiblioteQ Installer"
outFile "BiblioteQ-Installer-6.21.1.exe"

# Install directory.
installDir C:\BiblioteQ-6.21.1

DirText "Please choose an installation directory for BiblioteQ."

# Default section start.
section

# Define output path.
setOutPath $INSTDIR

# Specify files to go in output path.
file biblioteq.conf
file /r .\release\doc.d
file /r .\release\icons.d
file /r .\release\plugins

# The translations directory contains translation files
# provided by both BiblioteQ and Qt.

file /r .\release\translations
file .\release\*.dll
file .\release\qt.conf
file .\release\sqlite3.exe
file .\release\sqlite-*.zip
file .\release\biblioteq.db
file .\release\BiblioteQ.exe
file *.sql

# Define uninstaller name.
writeUninstaller $INSTDIR\BiblioteQ-Uninstaller-6.21.1.exe

# Default section end.
sectionEnd

# Create a section to define what the uninstaller does.
# The section will always be named "Uninstall".
section "Uninstall"

# Always delete uninstaller first.
RMDir /r $INSTDIR

sectionEnd
