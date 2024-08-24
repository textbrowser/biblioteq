include(biblioteq-source.pro)

greaterThan(QT_MAJOR_VERSION, 4) {
cache()
}

purge.commands = find . -name '*~*' -exec rm -f {} \;

CONFIG		+= copy_dir_files qt release warn_on
DEFINES		+= BIBLIOTEQ_CONFIGURATION_FILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QT		+= gui network printsupport sql widgets
QT		-= webkit

lessThan(QT_MAJOR_VERSION, 5) {
exists(/usr/include/poppler/qt4) {
DEFINES                += BIBLIOTEQ_LINKED_WITH_POPPLER
DEFINES                += BIBLIOTEQ_LINKED_WITH_POPPLER4
INCLUDEPATH            += /usr/include/poppler/qt4
LIBS                   += -lpoppler-qt4
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated-declarations
}
} else {
exists(/usr/include/poppler/qt5) {
DEFINES     += BIBLIOTEQ_LINKED_WITH_POPPLER
DEFINES     += BIBLIOTEQ_LINKED_WITH_POPPLER5
INCLUDEPATH += /usr/include/poppler/qt5
LIBS        += -lpoppler-qt5
}
}

greaterThan(QT_MAJOR_VERSION, 4) {
QT              += printsupport widgets
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
                          -pie
QMAKE_DISTCLEAN += -r Temporary

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS_RELEASE += -pedantic \
                          -std=c++11
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash
}

QMAKE_EXTRA_TARGETS = purge

ICON		= Icons/book.png
INCLUDEPATH	+= Source

exists(/usr/include/yaz) {
DEFINES         += BIBLIOTEQ_LINKED_WITH_YAZ
LIBS            += -lyaz
}

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app
