cache()
purge.commands = rm -f *~ && rm -f include.d/*~ && rm -f doc.d/*~

CONFIG		+= copy_dir_files qt release thread warn_on x11
DEFINES		+= CONFIGFILE="'\"/usr/local/biblioteq/biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= network printsupport sql widgets
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Woverloaded-virtual \
			  -Wpointer-arith -Wstrict-overflow=4 \
			  -Wstack-protector -fPIE -fstack-protector-all \
			  -fwrapv -mtune=generic -pie
QMAKE_DISTCLEAN	+= .qmake.cache .qmake.stash
QMAKE_EXTRA_TARGETS = purge

ICON		= icons.d/book.png
INCLUDEPATH	+= include.d /usr/local/include
LIBS		+= -Wl,-R/usr/local/lib -lsqlite3 -lyaz

FORMS           = adminsetup.ui \
		  allinfo.ui \
		  bookcopybrowser.ui \
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
		  photograph.ui \
		  photographinfo.ui \
		  photographview.ui \
		  sruResults.ui \
		  tracks.ui \
		  userinfo.ui \
		  videogameinfo.ui \
		  z3950results.ui

UI_HEADERS_DIR  = include.d

HEADERS		= include.d/bgraphicsscene.h \
		  include.d/borrowers_editor.h \
		  include.d/copy_editor.h \
		  include.d/copy_editor_book.h \
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
		  include.d/qtbook_photographcollection.h \
		  include.d/qtbook_videogame.h \
		  include.d/sruResults.h \
		  include.d/z3950results.h

SOURCES		= bgraphicsscene.cc \
		  borrowers_editor.cc \
		  callnum_table_item.cc \
		  copy_editor.cc \
		  copy_editor_book.cc \
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
		  sruResults.cc \
		  z3950results.cc

TRANSLATIONS    = translations.d/biblioteq_cs_CZ.ts \
		  translations.d/biblioteq_de_DE.ts \
		  translations.d/biblioteq_el_GR.ts \
		  translations.d/biblioteq_fr_FR.ts \
                  translations.d/biblioteq_nl_BE.ts \
                  translations.d/biblioteq_nl_NL.ts

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ

biblioteq.path		= /usr/local/biblioteq
biblioteq.files		= BiblioteQ
conf.path		= /usr/local/biblioteq
conf.files		= biblioteq.conf
icons.path		= /usr/local/biblioteq
icons.files		= icons.d
postinstall.path	= /usr/local/biblioteq
postinstall.extra	= find /usr/local/biblioteq -name .svn -exec rm -rf {} \\; 2>/dev/null; echo
sh.path			= /usr/local/biblioteq
sh.files		= biblioteq.sh
translations.path	= /usr/local/biblioteq/translations.d
translations.files	= translations.d/*.qm

INSTALLS	= icons \
                  translations \
                  biblioteq \
                  conf \
                  sh \
                  postinstall
