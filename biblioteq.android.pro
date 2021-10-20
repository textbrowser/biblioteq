cache()
include(biblioteq-source.pro)

purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= copy_dir_files qt release thread warn_on
DEFINES		+= BIBLIOTEQ_CONFIGFILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QMAKE_CLEAN	+= BiblioteQ
QT              += network printsupport sql widgets
QT              -= webkit
TEMPLATE	= app

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 \
                          -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Werror=format-security \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -std=c++11
QMAKE_DISTCLEAN += -r temp .qmake.cache .qmake.stash
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons/book.png
INCLUDEPATH	+= Source temp
LIBS		+=

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
