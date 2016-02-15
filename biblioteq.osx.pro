purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= BIBLIOTEQ_CONFIGFILE="'\"biblioteq.conf\"'" BIBLIOTEQ_WA_MACMETALSTYLE=1
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
QMAKE_DISTCLEAN += -r Include
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
QMAKE_STRIP	= echo

ICON		= Icons/book.icns
INCLUDEPATH	+= Include Source /usr/local/include
LIBS		+= -lsqlite3 -L/usr/local/lib -lyaz
RESOURCES	= Icons/icons.qrc

FORMS           = UI/biblioteq_adminsetup.ui \
		  UI/biblioteq_allinfo.ui \
		  UI/biblioteq_bookcopybrowser.ui \
		  UI/biblioteq_bookinfo.ui \
                  UI/biblioteq_borrowers.ui \
		  UI/biblioteq_branch_s.ui \
		  UI/biblioteq_cdinfo.ui \
		  UI/biblioteq_copybrowser.ui \
		  UI/biblioteq_customquery.ui \
		  UI/biblioteq_dbenumerations.ui \
		  UI/biblioteq_dvdinfo.ui \
		  UI/biblioteq_errordiag.ui \
		  UI/biblioteq_history.ui \
		  UI/biblioteq_maginfo.ui \
                  UI/biblioteq_mainwindow.ui \
		  UI/biblioteq_members_browser.ui \
		  UI/biblioteq_password.ui \
		  UI/biblioteq_passwordPrompt.ui \
		  UI/biblioteq_photograph.ui \
		  UI/biblioteq_photographinfo.ui \
		  UI/biblioteq_photographview.ui \
		  UI/biblioteq_sruResults.ui \
		  UI/biblioteq_tracks.ui \
		  UI/biblioteq_userinfo.ui \
		  UI/biblioteq_videogameinfo.ui \
		  UI/biblioteq_z3950results.ui

UI_HEADERS_DIR  = Include

HEADERS		= Source/biblioteq.h \
                  Source/biblioteq_bgraphicsscene.h \
		  Source/biblioteq_book.h \
		  Source/biblioteq_borrowers_editor.h \
		  Source/biblioteq_cd.h \
		  Source/biblioteq_copy_editor.h \
		  Source/biblioteq_copy_editor_book.h \
                  Source/biblioteq_dbenumerations.h \
		  Source/biblioteq_dvd.h \
                  Source/biblioteq_generic_thread.h \
		  Source/biblioteq_hyperlinked_text_edit.h \
		  Source/biblioteq_image_drop_site.h \
		  Source/biblioteq_item.h \
		  Source/biblioteq_magazine.h \
		  Source/biblioteq_main_table.h \
                  Source/biblioteq_myqstring.h \
		  Source/biblioteq_photographcollection.h \
		  Source/biblioteq_sruResults.h \
		  Source/biblioteq_videogame.h \
		  Source/biblioteq_z3950results.h

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
lrelease.extra          = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path           = .
lupdate.extra           = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
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
		  lupdate \
		  lrelease \
		  sql \
		  translations \
		  postinstall
