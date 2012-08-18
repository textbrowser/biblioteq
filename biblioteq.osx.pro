purge.commands = rm -f *~ && rm -f include.d/*~ && rm -f doc.d/*~

TEMPLATE	= app
LANGUAGE	= C++
QT		+= sql network
CONFIG		+= qt release thread warn_on app_bundle
DEFINES		+= CONFIGFILE="'\"biblioteq.conf\"'"
QMAKE_LFLAGS_RELEASE =
QMAKE_LFLAGS_RPATH =
QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Wall -Wextra -Werror -mtune=generic -O3
QMAKE_EXTRA_TARGETS = purge
INCLUDEPATH	+= include.d /usr/local/include
ICON		= icons.d/book.icns
LIBS		+= -lsqlite3 \
		   -L/usr/local/lib -lyaz \
		   -L/Library/PostgreSQL/9.1/lib \
		   -lpq.5 -lcrypto.1.0.0 -lssl.1.0.0

FORMS           = adminsetup.ui \
		  allinfo.ui \
		  bookinfo.ui \
                  borrowers.ui \
		  branch_s.ui \
		  cdinfo.ui \
		  copybrowser.ui \
		  customquery.ui \
		  dbenumerations.ui \
		  dvdinfo.ui \
		  errordiag.ui \
		  history.ui \
		  maginfo.ui \
                  mainwindow.ui \
		  members_browser.ui \
		  password.ui \
		  passwordPrompt.ui \
		  tracks.ui \
		  userinfo.ui \
		  videogameinfo.ui \
		  z3950results.ui

UI_HEADERS_DIR  = include.d

HEADERS		= include.d/bgraphicsscene.h \
		  include.d/borrowers_editor.h \
		  include.d/copy_editor.h \
                  include.d/dbenumerations.h \
		  include.d/generic_thread.h \
		  include.d/hyperlinked_text_edit.h \
		  include.d/image_drop_site.h \
		  include.d/main_table.h \
                  include.d/myqstring.h \
		  include.d/qtbook_book.h \
		  include.d/qtbook_cd.h \
		  include.d/qtbook_dvd.h \
		  include.d/qtbook.h \
		  include.d/qtbook_item.h \
		  include.d/qtbook_magazine.h \
		  include.d/qtbook_videogame.h \
		  include.d/z3950results.h

SOURCES		= bgraphicsscene.cc \
		  borrowers_editor.cc \
		  callnum_table_item.cc \
		  copy_editor.cc \
                  dbenumerations.cc \
		  generic_thread.cc \
		  hyperlinked_text_edit.cc \
		  image_drop_site.cc \
		  main_table.cc \
		  misc_functions.cc \
                  myqstring.cc \
		  numeric_table_item.cc \
		  qtbook_a.cc \
		  qtbook_b.cc \
		  qtbook_book.cc \
		  qtbook_cd.cc \
		  qtbook_dvd.cc \
		  qtbook_item.cc \
		  qtbook_journal.cc \
		  qtbook_magazine.cc \
		  qtbook_videogame.cc \
		  z3950results.cc

TRANSLATIONS    = translations.d/biblioteq_cs_CZ.ts \
		  translations.d/biblioteq_de_DE.ts \
		  translations.d/biblioteq_el_GR.ts \
		  translations.d/biblioteq_fr_FR.ts \
                  translations.d/biblioteq_nl_BE.ts \
                  translations.d/biblioteq_nl_NL.ts

TARGET		= BiblioteQ
PROJECTNAME	= BiblioteQ

biblioteq.path		= /Applications/BiblioteQ.d/BiblioteQ.app
biblioteq.files		= BiblioteQ.app/*
conf.path		= /Applications/BiblioteQ.d
conf.files		= biblioteq.conf
doc.path		= /Applications/BiblioteQ.d/doc.d
doc.files		= doc.d/*.pdf doc.d/TO-DO
icons.path		= /Applications/BiblioteQ.d
icons.files		= icons.d
macdeployqt.path	= BiblioteQ.app
macdeployqt.extra	= /opt/QtSDK/Desktop/Qt/474/gcc/bin/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
postinstall.path	= /Applications/BiblioteQ.d
postinstall.extra	= find /Applications/BiblioteQ.d -name .svn -exec rm -rf {} \\; 2>/dev/null; echo
preinstall.path         = /Applications/BiblioteQ.d
preinstall.extra        = rm -rf /Applications/BiblioteQ.d/BiblioteQ.app/*
sql.path		= /Applications/BiblioteQ.d
sql.files		= *.sql
translations.path	= /Applications/BiblioteQ.d/translations.d
translations.files	= translations.d/*.qm

QMAKE_STRIP	= echo
INSTALLS	= preinstall \
		  macdeployqt \
		  conf \
		  doc \
		  icons \
		  sql \
		  translations \
                  biblioteq \
		  postinstall
