purge.commands = del *~ && del include.d\\*~ && del doc.d\\*~

TEMPLATE	= app
LANGUAGE	= C++
QT		+= sql network
CONFIG		+= qt release thread warn_on windows
DEFINES		+=
QMAKE_CXXFLAGS	+= -Wall -Wextra -Werror
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE += -Wall -Wextra -Werror
QMAKE_EXTRA_TARGETS = purge
INCLUDEPATH	+= include.d include.win32.d
ICON		= icons.d\\book.png
RC_FILE		= biblioteq.win.rc
LIBS		+= -L"." \
		   -L"libraries.win32.d\\sqlite3.d" \
		   -L"libraries.win32.d\\yaz.d" -lsqlite3 -lyaz4

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
		  photographinfo.ui \
		  tracks.ui \
		  userinfo.ui \
		  videogameinfo.ui \
		  z3950results.ui

UI_HEADERS_DIR  = include.d

HEADERS		= include.d\\bgraphicsscene.h \
		  include.d\\borrowers_editor.h \
		  include.d\\copy_editor.h \
                  include.d\\dbenumerations.h \
		  include.d\\generic_thread.h \
		  include.d\\hyperlinked_text_edit.h \
		  include.d\\image_drop_site.h \
		  include.d\\main_table.h \
                  include.d\\myqstring.h \
		  include.d\\qtbook_book.h \
		  include.d\\qtbook_cd.h \
		  include.d\\qtbook_dvd.h \
		  include.d\\qtbook.h \
		  include.d\\qtbook_item.h \
		  include.d\\qtbook_magazine.h \
		  include.d\\qtbook_photographcollection.h \
		  include.d\\qtbook_videogame.h \
		  include.d\\z3950results.h

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
		  qtbook_photographcollection.cc \
		  qtbook_videogame.cc \
		  z3950results.cc

TRANSLATIONS    = translations.d\\biblioteq_cs_CZ.ts \
		  translations.d\\biblioteq_de_DE.ts \
		  translations.d\\biblioteq_el_GR.ts \
		  translations.d\\biblioteq_fr_FR.ts \
                  translations.d\\biblioteq_nl_BE.ts \
                  translations.d\\biblioteq_nl_NL.ts

TARGET		= BiblioteQ
PROJECTNAME	= BiblioteQ
