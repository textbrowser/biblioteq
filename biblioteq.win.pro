purge.commands = del *~ && del *\\*~

CONFIG		+= qt release thread warn_on windows
DEFINES		+=
LANGUAGE	= C++
QT		+= network sql
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual \
			  -Werror -Wextra -Wformat=2 \
			  -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -fwrapv -mtune=generic -pie
QMAKE_DISTCLEAN += -r Include
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons\\book.png
INCLUDEPATH	+= Include Source Include.win32
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

HEADERS		= Source\\biblioteq.h \
                  Source\\biblioteq_bgraphicsscene.h \
		  Source\\biblioteq_book.h \
		  Source\\biblioteq_borrowers_editor.h \
		  Source\\biblioteq_cd.h \
		  Source\\biblioteq_copy_editor.h \
		  Source\\biblioteq_copy_editor_book.h \
                  Source\\biblioteq_dbenumerations.h \
		  Source\\biblioteq_dvd.h \
                  Source\\biblioteq_generic_thread.h \
		  Source\\biblioteq_hyperlinked_text_edit.h \
		  Source\\biblioteq_image_drop_site.h \
		  Source\\biblioteq_item.h \
		  Source\\biblioteq_magazine.h \
		  Source\\biblioteq_main_table.h \
                  Source\\biblioteq_myqstring.h \
		  Source\\biblioteq_photographcollection.h \
		  Source\\biblioteq_sruResults.h \
		  Source\\biblioteq_videogame.h \
		  Source\\biblioteq_z3950results.h

SOURCES		= Source\\biblioteq_a.cc \
                  Source\\biblioteq_b.cc \
                  Source\\biblioteq_bgraphicsscene.cc \
                  Source\\biblioteq_book.cc \
                  Source\\biblioteq_borrowers_editor.cc \
                  Source\\biblioteq_callnum_table_item.cc \
                  Source\\biblioteq_cd.cc \
                  Source\\biblioteq_copy_editor.cc \
                  Source\\biblioteq_copy_editor_book.cc \
                  Source\\biblioteq_dbenumerations.cc \
                  Source\\biblioteq_dvd.cc \
                  Source\\biblioteq_generic_thread.cc \
                  Source\\biblioteq_hyperlinked_text_edit.cc \
                  Source\\biblioteq_image_drop_site.cc \
                  Source\\biblioteq_item.cc \
                  Source\\biblioteq_journal.cc \
                  Source\\biblioteq_magazine.cc \
                  Source\\biblioteq_main_table.cc \
                  Source\\biblioteq_marc.cc \
                  Source\\biblioteq_misc_functions.cc \
                  Source\\biblioteq_myqstring.cc \
                  Source\\biblioteq_numeric_table_item.cc \
                  Source\\biblioteq_photographcollection.cc \
                  Source\\biblioteq_sruResults.cc \
                  Source\\biblioteq_videogame.cc \
                  Source\\biblioteq_z3950results.cc

TRANSLATIONS    = Translations\\biblioteq_cs_CZ.ts \
		  Translations\\biblioteq_de_DE.ts \
		  Translations\\biblioteq_el_GR.ts \
		  Translations\\biblioteq_fr_FR.ts \
		  Translations\\biblioteq_hu_HU.ts \
                  Translations\\biblioteq_nl_BE.ts \
                  Translations\\biblioteq_nl_NL.ts

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
