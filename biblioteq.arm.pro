purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= copy_dir_files qt release thread warn_on
DEFINES		+= CONFIGFILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= network sql
TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Wformat=2 -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
			  -fwrapv -mtune=native -pie
QMAKE_EXTRA_TARGETS = purge
QMAKE_STRIP	= echo

ICON		= Icons/book.png
INCLUDEPATH	+= Include
LIBS		+= -lsqlite3 -lyaz
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

biblioteq.path		= /usr/local/biblioteq
biblioteq.files		= BiblioteQ
conf.path		= /usr/local/biblioteq
conf.files		= biblioteq.conf
sh.path			= /usr/local/biblioteq
sh.files		= biblioteq.sh
translations.path	= /usr/local/biblioteq/Translations
translations.files	= Translations/*.qm

INSTALLS	= biblioteq \
		  conf \
		  sh \
                  translations
