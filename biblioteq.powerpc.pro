cache()
include(biblioteq-source.pro)

purge.commands = find . -name \'*~*\' -exec rm -f {} \;

CONFIG		+= copy_dir_files qt release warn_on
DEFINES		+= BIBLIOTEQ_CONFIGURATION_FILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= gui network printsupport sql widgets
QT		-= webkit

exists(/usr/include/poppler/qt5) {
DEFINES     += BIBLIOTEQ_LINKED_WITH_POPPLER
DEFINES     += BIBLIOTEQ_LINKED_WITH_POPPLER5
INCLUDEPATH += /usr/include/poppler/qt5
LIBS        += -lpoppler-qt5
}

exists(/usr/include/yaz) {
DEFINES         += BIBLIOTEQ_LINKED_WITH_YAZ
LIBS            += -lyaz
}

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wextra \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -mabi=altivec \
                          -mcpu=powerpc \
                          -mtune=powerpc \
                          -pedantic \
                          -pie \
                          -std=c++11
QMAKE_DISTCLEAN += -r .qmake.cache .qmake.stash Temporary
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons/book.png
INCLUDEPATH	+= Source
PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app
