# Define installer name.
Name "BiblioteQ Installer"
outFile "BiblioteQ-Installer-6.20.2.exe"

# Install directory.
installDir C:\BiblioteQ-6.20.2

DirText "Please choose an installation directory for BiblioteQ."

# Default section start.
section

# Define output path.
setOutPath $INSTDIR

# Specify files to go in output path.
file biblioteq.conf
file /r .\release\icons.d
file /r .\release\plugins
file /r .\release\doc.d
file .\release\*.dll
file .\release\qt.conf
file .\release\biblioteq.db
file .\release\BiblioteQ.exe
file .\release\sqlite3.exe
file .\release\sqlite-*.zip
file *.sql

# Define uninstaller name.
writeUninstaller $INSTDIR\BiblioteQ-Uninstaller-6.20.2.exe

# Default section end.
sectionEnd

# Create a section to define what the uninstaller does.
# The section will always be named "Uninstall".
section "Uninstall"

# Always delete uninstaller first.
RMDir /r $INSTDIR

sectionEnd
