purge.commands = del *~ && del *\\*~

CONFIG		+= qt release thread warn_on windows
DEFINES		+=
LANGUAGE	= C++
QT		+= network sql
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os -Wall -Wcast-align -Wcast-qual \
			  -Werror -Wextra \
			  -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -fwrapv -mtune=generic -pie
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons\\book.png
INCLUDEPATH	+= Include Include.win32
LIBS		+= -L"." \
		   -L"Libraries.win32\\sqlite3.d" \
		   -L"Libraries.win32\\yaz.d" -lsqlite3 -lyaz5
RC_FILE		= biblioteq.win.rc
RESOURCES	= Icons\\icons.qrc

FORMS           = UI\\adminsetup.ui \
		  UI\\allinfo.ui \
                  UI\\bookcopybrowser.ui \
                  UI\\bookinfo.ui \
                  UI\\borrowers.ui \
                  UI\\branch_s.ui \
                  UI\\cdinfo.ui \
                  UI\\copybrowser.ui \
                  UI\\customquery.ui \
                  UI\\dbenumerations.ui \
                  UI\\dvdinfo.ui \
                  UI\\errordiag.ui \
                  UI\\history.ui \
                  UI\\maginfo.ui \
                  UI\\mainwindow.ui \
                  UI\\members_browser.ui \
                  UI\\password.ui \
                  UI\\passwordPrompt.ui \
                  UI\\photograph.ui \
                  UI\\photographinfo.ui \
                  UI\\photographview.ui \
                  UI\\sruResults.ui \
                  UI\\tracks.ui \
                  UI\\userinfo.ui \
                  UI\\videogameinfo.ui \
                  UI\\z3950results.ui

UI_HEADERS_DIR  = Include

HEADERS		= Include\\bgraphicsscene.h \
		  Include\\borrowers_editor.h \
		  Include\\copy_editor.h \
		  Include\\copy_editor_book.h \
                  Include\\dbenumerations.h \
		  Include\\generic_thread.h \
		  Include\\hyperlinked_text_edit.h \
		  Include\\image_drop_site.h \
		  Include\\main_table.h \
                  Include\\myqstring.h \
		  Include\\qtbook_book.h \
		  Include\\qtbook_cd.h \
		  Include\\qtbook_dvd.h \
		  Include\\qtbook.h \
		  Include\\qtbook_item.h \
		  Include\\qtbook_magazine.h \
		  Include\\qtbook_photographcollection.h \
		  Include\\qtbook_videogame.h \
		  Include\\sruResults.h \
		  Include\\z3950results.h

SOURCES		= Source\\bgraphicsscene.cc \
		  Source\\borrowers_editor.cc \
                  Source\\callnum_table_item.cc \
                  Source\\copy_editor.cc \
                  Source\\copy_editor_book.cc \
                  Source\\dbenumerations.cc \
                  Source\\generic_thread.cc \
                  Source\\hyperlinked_text_edit.cc \
                  Source\\image_drop_site.cc \
                  Source\\main_table.cc \
		  Source\\marc.cc \
                  Source\\misc_functions.cc \
                  Source\\myqstring.cc \
                  Source\\numeric_table_item.cc \
                  Source\\qtbook_a.cc \
                  Source\\qtbook_b.cc \
                  Source\\qtbook_book.cc \
                  Source\\qtbook_cd.cc \
                  Source\\qtbook_dvd.cc \
                  Source\\qtbook_item.cc \
                  Source\\qtbook_journal.cc \
                  Source\\qtbook_magazine.cc \
                  Source\\qtbook_photographcollection.cc \
                  Source\\qtbook_videogame.cc \
                  Source\\sruResults.cc \
                  Source\\z3950results.cc

TRANSLATIONS    = Translations\\biblioteq_cs_CZ.ts \
		  Translations\\biblioteq_de_DE.ts \
		  Translations\\biblioteq_el_GR.ts \
		  Translations\\biblioteq_fr_FR.ts \
		  Translations\\biblioteq_hu_HU.ts \
                  Translations\\biblioteq_nl_BE.ts \
                  Translations\\biblioteq_nl_NL.ts

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
