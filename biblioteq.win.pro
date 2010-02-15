purge.commands = del *~ && del include.d\*~ && del doc.d\*~

TEMPLATE	= app
LANGUAGE	= C++
QT		+= sql network
CONFIG		+= qt release thread warn_on windows
DEFINES		+=
QMAKE_CXXFLAGS	+= -Wall -Werror
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CFLAGS_RELEASE += -Werror
QMAKE_CXXFLAGS_RELEASE += -Werror
QMAKE_EXTRA_WINDOWS_TARGETS = purge
INCPATH		+= include.d "C:\Program Files\YAZ\include"
ICON		= icons.d\book.png
RC_FILE		= biblioteq.win.rc
LIBS		+= -L"C:\Program Files\YAZ\lib" -lyaz3

FORMS           = cdinfo.ui \
		  tracks.ui \
		  allinfo.ui \
		  dvdinfo.ui \
		  history.ui \
		  maginfo.ui \
		  bookinfo.ui \
		  branch_s.ui \
		  password.ui \
		  userinfo.ui \
                  borrowers.ui \
		  errordiag.ui \
		  newsqlite.ui \
		  adminsetup.ui \
                  mainwindow.ui \
		  copybrowser.ui \
		  customquery.ui \
		  z3950results.ui \
		  videogameinfo.ui \
		  members_browser.ui

UI_HEADERS_DIR  = include.d

HEADERS		= include.d\qtbook.h \
                  include.d\myqstring.h \
		  include.d\qtbook_cd.h \
		  include.d\main_table.h \
		  include.d\qtbook_dvd.h \
		  include.d\copy_editor.h \
		  include.d\qtbook_book.h \
		  include.d\qtbook_item.h \
		  include.d\z3950results.h \
		  include.d\image_drop_site.h \
		  include.d\qtbook_magazine.h \
		  include.d\borrowers_editor.h \
		  include.d\qtbook_videogame.h \
		  include.d\hyperlinked_text_edit.h

SOURCES		= qtbook.cc \
                  myqstring.cc \
		  qtbook_cd.cc \
		  main_table.cc \
		  qtbook_dvd.cc \
		  copy_editor.cc \
		  qtbook_book.cc \
		  qtbook_item.cc \
		  z3950results.cc \
		  generic_thread.cc \
		  misc_functions.cc \
		  qtbook_journal.cc \
		  image_drop_site.cc \
		  qtbook_magazine.cc \
		  borrowers_editor.cc \
		  qtbook_videogame.cc \
		  numeric_table_item.cc \
		  hyperlinked_text_edit.cc

TRANSLATIONS    = translations.d\biblioteq_cs_CZ.ts \
		  translations.d\biblioteq_de_DE.ts

TARGET		= BiblioteQ
PROJECTNAME	= BiblioteQ
