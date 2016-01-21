purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= CONFIGFILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= network sql
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Wformat=2 -Woverloaded-virtual \
			  -Wpointer-arith -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
			  -fwrapv -mtune=generic -pie
QMAKE_EXTRA_TARGETS = purge
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
QMAKE_STRIP	= echo

ICON		= Icons/book.icns
INCLUDEPATH	+= Include /usr/local/include
LIBS		+= -lsqlite3 -L/usr/local/lib -lyaz
RESOURCES	= Icons/icons.qrc

FORMS           = UI/adminsetup.ui \
		  UI/allinfo.ui \
		  UI/bookcopybrowser.ui \
		  UI/bookinfo.ui \
                  UI/borrowers.ui \
		  UI/branch_s.ui \
		  UI/cdinfo.ui \
		  UI/copybrowser.ui \
		  UI/customquery.ui \
		  UI/dbenumerations.ui \
		  UI/dvdinfo.ui \
		  UI/errordiag.ui \
		  UI/history.ui \
		  UI/maginfo.ui \
                  UI/mainwindow.ui \
		  UI/members_browser.ui \
		  UI/password.ui \
		  UI/passwordPrompt.ui \
		  UI/photograph.ui \
		  UI/photographinfo.ui \
		  UI/photographview.ui \
		  UI/sruResults.ui \
		  UI/tracks.ui \
		  UI/userinfo.ui \
		  UI/videogameinfo.ui \
		  UI/z3950results.ui

UI_HEADERS_DIR  = Include

HEADERS		= Include/biblioteq.h \
                  Include/biblioteq_bgraphicsscene.h \
		  Include/biblioteq_book.h \
		  Include/biblioteq_borrowers_editor.h \
		  Include/biblioteq_cd.h \
		  Include/biblioteq_copy_editor.h \
		  Include/biblioteq_copy_editor_book.h \
                  Include/biblioteq_dbenumerations.h \
		  Include/biblioteq_dvd.h \
                  Include/biblioteq_generic_thread.h \
		  Include/biblioteq_hyperlinked_text_edit.h \
		  Include/biblioteq_image_drop_site.h \
		  Include/biblioteq_item.h \
		  Include/biblioteq_magazine.h \
		  Include/biblioteq_main_table.h \
                  Include/biblioteq_myqstring.h \
		  Include/biblioteq_photographcollection.h \
		  Include/biblioteq_sruResults.h \
		  Include/biblioteq_videogame.h \
		  Include/biblioteq_z3950results.h

SOURCES		= Source/biblioteq_a.cc \
                  Source/biblioteq_b.cc \
                  Source/biblioteq_bgraphicsscene.cc \
                  Source/biblioteq_book.cc \
                  Source/biblioteq_borrowers_editor.cc \
                  Source/biblioteq_callnum_table_item.cc \
                  Source/biblioteq_cd.cc \
                  Source/biblioteq_copy_editor.cc \
                  Source/biblioteq_copy_editor_book.cc \
                  Source/biblioteq_dbenumerations.cc \
                  Source/biblioteq_dvd.cc \
                  Source/biblioteq_generic_thread.cc \
                  Source/biblioteq_hyperlinked_text_edit.cc \
                  Source/biblioteq_image_drop_site.cc \
                  Source/biblioteq_item.cc \
                  Source/biblioteq_journal.cc \
                  Source/biblioteq_magazine.cc \
                  Source/biblioteq_main_table.cc \
                  Source/biblioteq_marc.cc \
                  Source/biblioteq_misc_functions.cc \
                  Source/biblioteq_myqstring.cc \
                  Source/biblioteq_numeric_table_item.cc \
                  Source/biblioteq_photographcollection.cc \
                  Source/biblioteq_sruResults.cc \
                  Source/biblioteq_videogame.cc \
                  Source/biblioteq_z3950results.cc

TRANSLATIONS    = Translations/biblioteq_cs_CZ.ts \
		  Translations/biblioteq_de_DE.ts \
		  Translations/biblioteq_el_GR.ts \
		  Translations/biblioteq_fr_FR.ts \
		  Translations/biblioteq_hu_HU.ts \
                  Translations/biblioteq_nl_BE.ts \
                  Translations/biblioteq_nl_NL.ts

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ

biblioteq.path		= /Applications/BiblioteQ.d/BiblioteQ.app
biblioteq.files		= BiblioteQ.app/*
conf.path		= /Applications/BiblioteQ.d
conf.files		= biblioteq.conf
doc1.path		= /Applications/BiblioteQ.d/Documentation
doc1.files		= Documentation/*.pdf Documentation/*.txt Documentation/TO-DO
doc2.path		= /Applications/BiblioteQ.d/Documentation/Contributed
doc2.files		= Documentation/Contributed/*.docx Documentation/Contributed/*.pdf
macdeployqt.path	= BiblioteQ.app
macdeployqt.extra	= $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
preinstall.path         = /Applications/BiblioteQ.d
preinstall.extra        = rm -rf /Applications/BiblioteQ.d/BiblioteQ.app/*
postinstall.path	= /Applications/BiblioteQ.d
postinstall.extra	= cp -r BiblioteQ.app /Applications/BiblioteQ.d/.
sql.path		= /Applications/BiblioteQ.d
sql.files		= SQL/*.sql
translations.path	= /Applications/BiblioteQ.d/Translations
translations.files	= Translations/*.qm

INSTALLS	= preinstall \
		  macdeployqt \
		  biblioteq \
		  conf \
		  doc1 \
		  doc2 \
		  sql \
		  translations \
		  postinstall
