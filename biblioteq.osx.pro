purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= CONFIGFILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= network sql
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Woverloaded-virtual \
			  -Wpointer-arith -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
			  -fwrapv -mtune=generic -pie
QMAKE_EXTRA_TARGETS = purge
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

HEADERS		= Include/bgraphicsscene.h \
		  Include/borrowers_editor.h \
		  Include/copy_editor.h \
		  Include/copy_editor_book.h \
                  Include/dbenumerations.h \
		  Include/generic_thread.h \
		  Include/hyperlinked_text_edit.h \
		  Include/image_drop_site.h \
		  Include/main_table.h \
                  Include/myqstring.h \
		  Include/qtbook_book.h \
		  Include/qtbook_cd.h \
		  Include/qtbook_dvd.h \
		  Include/qtbook.h \
		  Include/qtbook_item.h \
		  Include/qtbook_magazine.h \
		  Include/qtbook_photographcollection.h \
		  Include/qtbook_videogame.h \
		  Include/sruResults.h \
		  Include/z3950results.h

SOURCES		= Source/bgraphicsscene.cc \
		  Source/borrowers_editor.cc \
		  Source/callnum_table_item.cc \
		  Source/copy_editor.cc \
		  Source/copy_editor_book.cc \
                  Source/dbenumerations.cc \
                  Source/generic_thread.cc \
                  Source/hyperlinked_text_edit.cc \
                  Source/image_drop_site.cc \
                  Source/main_table.cc \
                  Source/misc_functions.cc \
                  Source/myqstring.cc \
                  Source/numeric_table_item.cc \
                  Source/qtbook_a.cc \
                  Source/qtbook_b.cc \
                  Source/qtbook_book.cc \
                  Source/qtbook_cd.cc \
                  Source/qtbook_dvd.cc \
		  Source/qtbook_item.cc \
		  Source/qtbook_journal.cc \
		  Source/qtbook_magazine.cc \
		  Source/qtbook_photographcollection.cc \
		  Source/qtbook_videogame.cc \
		  Source/sruResults.cc \
		  Source/z3950results.cc

TRANSLATIONS    = Translations/biblioteq_cs_CZ.ts \
		  Translations/biblioteq_de_DE.ts \
		  Translations/biblioteq_el_GR.ts \
		  Translations/biblioteq_fr_FR.ts \
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
aftermacdeployqt1.path  = BiblioteQ.app/Contents/lib
aftermacdeployqt1.extra = cp Libraries.osx64/*.dylib ./BiblioteQ.app/Contents/lib/.
aftermacdeployqt2.path  = .
aftermacdeployqt2.extra = install_name_tool -change /usr/lib/libpq.5.dylib @loader_path/../../lib/libpq.5.5.dylib BiblioteQ.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib
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
                  aftermacdeployqt1 \
                  aftermacdeployqt2 \
		  biblioteq \
		  conf \
		  doc1 \
		  doc2 \
		  sql \
		  translations \
		  postinstall
