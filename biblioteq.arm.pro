cache()
include(biblioteq-source.pro)
purge.commands = find . -name '*~*' -exec rm -f {} \;

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

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-deprecated-declarations \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
QMAKE_DISTCLEAN += -r .qmake.cache .qmake.stash Temporary
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons/book.png
INCLUDEPATH	+= Source
PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app
