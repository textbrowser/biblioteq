cache()
include(biblioteq-source.pro)
purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXX       = clang++
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
                          -Wstrict-overflow=5 \
                          -fPIE \
                          -fstack-protector-all \
                          -fwrapv \
                          -mtune=generic \
                          -pedantic \
                          -std=c++11
QT		+= network printsupport sql widgets
QT		-= webkit

QMAKE_EXTRA_TARGETS = purge
QMAKE_DISTCLEAN += -r temp
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

ICON		= Icons/book.icns
INCLUDEPATH	+= /usr/local/include \
                   Source \
                   temp
LIBS            += -framework Cocoa
LIBS		+= -L/usr/local/lib \
                   -lpq \
                   -lsqlite3 \
                   -lyaz

OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
PROJECTNAME	= BiblioteQ
RESOURCES	= Icons/icons.qrc
TARGET		= BiblioteQ
TEMPLATE        = app

biblioteq.files		= BiblioteQ.app/*
biblioteq.path		= /Applications/BiblioteQ.Qt5_9.d/BiblioteQ.app
conf.files		= biblioteq.conf
conf.path		= /Applications/BiblioteQ.Qt5_9.d
doc1.files		= Documentation/*.pdf Documentation/*.txt Documentation/TO-DO
doc1.path		= /Applications/BiblioteQ.Qt5_9.d/Documentation
doc2.files		= Documentation/Contributed/*.docx Documentation/Contributed/*.pdf
doc2.path		= /Applications/BiblioteQ.Qt5_9.d/Documentation/Contributed
lrelease.extra          = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path           = .
lupdate.extra           = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
lupdate.path            = .
macdeployqt.extra	= $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
macdeployqt.path	= BiblioteQ.app
postinstall.extra	= cp -r BiblioteQ.app /Applications/BiblioteQ.Qt5_9.d/.
postinstall.path	= /Applications/BiblioteQ.Qt5_9.d
preinstall.extra        = rm -rf /Applications/BiblioteQ.Qt5_9.d/BiblioteQ.app/*
preinstall.path         = /Applications/BiblioteQ.Qt5_9.d
sql.files		= SQL/*.sql
sql.path		= /Applications/BiblioteQ.Qt5_9.d
translations.files	= Translations/*.qm
translations.path	= /Applications/BiblioteQ.Qt5_9.d/Translations

INSTALLS	= preinstall \
		  macdeployqt \
		  biblioteq \
		  conf \
		  doc1 \
		  doc2 \
		  sql \
		  translations \
                  postinstall
