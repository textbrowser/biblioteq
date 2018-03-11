include(biblioteq-source.pro)

greaterThan(QT_MAJOR_VERSION, 4) {
cache()
}

doxygen.commands = doxygen biblioteq.doxygen
purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= copy_dir_files qt release thread warn_on
DEFINES		+= BIBLIOTEQ_CONFIGFILE="'\"biblioteq.conf\"'" \
                   QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QT		-= webkit
QT              += network sql

lessThan(QT_MAJOR_VERSION, 5) {
exists(/usr/include/poppler/qt4) {
DEFINES +=      BIBLIOTEQ_LINKED_WITH_POPPLER
INCLUDEPATH     += /usr/include/poppler/qt4
LIBS    +=      -lpoppler-qt4
}
}
else {
exists(/usr/include/poppler/qt5) {
DEFINES +=      BIBLIOTEQ_LINKED_WITH_POPPLER
INCLUDEPATH     += /usr/include/poppler/qt5
LIBS    +=      -lpoppler-qt5
}
}

exists(/usr/include/poppler/cpp) {
DEFINES +=     BIBLIOTEQ_POPPLER_VERSION_DEFINED
INCLUDEPATH += /usr/include/poppler/cpp
}
else {
message("The directory /usr/include/poppler/cpp does not exist. Poppler version information will not be available.")
}

greaterThan(QT_MAJOR_VERSION, 4) {
QT              += printsupport widgets
}

TEMPLATE	= app

QMAKE_CLEAN	+= BiblioteQ

openbsd-* {
}
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Wformat=2 -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
                          -fwrapv -pie
else {
QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual -Werror -Wextra \
			  -Wformat=2 -Woverloaded-virtual -Wpointer-arith \
			  -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
                          -fwrapv -mtune=generic -pie
}

QMAKE_DISTCLEAN += -r temp

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash
}

QMAKE_EXTRA_TARGETS = doxygen purge

ICON		= Icons/book.png
INCLUDEPATH	+= Source temp
LIBS		+= -lsqlite3 -lyaz
RESOURCES	= Icons/icons.qrc
PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ

biblioteq.path		= /usr/local/biblioteq
biblioteq.files		= BiblioteQ
conf.path		= /usr/local/biblioteq
conf.files		= biblioteq.conf
lrelease.extra          = $$[QT_INSTALL_BINS]/lrelease biblioteq.pro
lrelease.path           = .
lupdate.extra           = $$[QT_INSTALL_BINS]/lupdate biblioteq.pro
lupdate.path            = .
sh.path			= /usr/local/biblioteq
sh.files                = biblioteq.sh

greaterThan(QT_MAJOR_VERSION, 4) {
sh.files		= biblioteq.qt5.sh
}

translations.path	= /usr/local/biblioteq/Translations
translations.files	= Translations/*.qm

INSTALLS	= biblioteq \
                  conf \
                  lupdate \
                  lrelease \
                  sh \
                  translations
