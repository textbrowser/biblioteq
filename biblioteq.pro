cache()
include(biblioteq-source.pro)

purge.commands = find . -name \'*~*\' -exec rm -f {} \;

CONFIG	    += copy_dir_files qt release warn_on
DEFINES	    += BIBLIOTEQ_CONFIGURATION_FILE="'\"biblioteq.conf\"'" \
               QT_DEPRECATED_WARNINGS
LANGUAGE    = C++
QMAKE_CLEAN += BiblioteQ
QT          += gui network printsupport sql widgets
QT          -= webkit
TEMPLATE    = app

freebsd-* | openbsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
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
                          -std=c++17
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Werror=format-security \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat-overflow=2 \
                          -Wformat=2 \
                          -Wlogical-op \
                          -Wno-deprecated-declarations \
                          -Wold-style-cast \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wtrampolines \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
}

exists(/usr/include/sqlite3.h) {
DEFINES += BIBLIOTEQ_SQLITE3_INCLUDE_FILE_EXISTS
LIBS    += -lsqlite3
}

QMAKE_DISTCLEAN     += -r .qmake.cache .qmake.stash Temporary
QMAKE_EXTRA_TARGETS = purge

ICON	    = Icons/book.png
INCLUDEPATH += Source
PROJECTNAME = BiblioteQ
TARGET	    = BiblioteQ
