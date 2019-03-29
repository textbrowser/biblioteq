include(biblioteq-source.pro)

greaterThan(QT_MAJOR_VERSION, 4) {
cache()
}

purge.commands = del *~ && del *\\*~

CONFIG		+= qt release thread warn_on windows
DEFINES		+= BIBLIOTEQ_LINKED_WITH_POPPLER \
                   BIBLIOTEQ_POPPLER_VERSION_DEFINED
LANGUAGE	= C++
QT		-= webkit
QT		+= network sql

greaterThan(QT_MAJOR_VERSION, 4) {
QT              += printsupport widgets
}

QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual \
			  -Wextra -Wformat=2 \
			  -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -fwrapv -mtune=generic -pie
QMAKE_DISTCLEAN += -r temp

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash
}

QMAKE_EXTRA_TARGETS = purge

ICON		= Icons\\book.png
INCLUDEPATH	+= Source Include.win32 Include.win32\\poppler.d\\cpp \
                   Include.win32\\poppler.d\\qt5 temp
LIBS		+= -L"." \
                   -L"Libraries.win32\\poppler.d" \
		   -L"Libraries.win32\\sqlite3.d" \
                   -L"Libraries.win32\\yaz.d" \
                   -lpoppler -lpoppler-qt5 -lsqlite3 -lyaz5
RC_FILE		= biblioteq.win.rc
RESOURCES	= Icons\\icons.qrc
PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app
