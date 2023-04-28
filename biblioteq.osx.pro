include(biblioteq-source.pro)
purge.commands = find . -name '*~*' -exec rm -f {} \;

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QT		+= network sql printsupport widgets
QT		-= webkit

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXX       = clang++
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
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
                          -pedantic \
                          -std=c++17
QMAKE_DISTCLEAN += -r BiblioteQ.d temp .qmake.cache .qmake.stash
QMAKE_EXTRA_TARGETS = purge
QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0

ICON		= Icons/book.icns
INCLUDEPATH	+= /usr/local/include \
                   Source \
                   temp
LIBS		+= -L/usr/local/lib \
                   -framework Cocoa

OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app

biblioteq.files		 = BiblioteQ.app/*
biblioteq.path		 = BiblioteQ.d/BiblioteQ.app
conf.files		 = biblioteq.conf
conf.path		 = BiblioteQ.d
data.files		 = Data/*
data.path		 = BiblioteQ.d/Data
doc1.files		 = Documentation/*.html Documentation/*.pdf Documentation/*.txt Documentation/TO-DO
doc1.path		 = BiblioteQ.d/Documentation
doc2.files		 = Documentation/Contributed/*.docx Documentation/Contributed/*.html Documentation/Contributed/*.pdf
doc2.path		 = BiblioteQ.d/Documentation/Contributed
lrelease.extra           = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path            = .
lupdate.extra            = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
lupdate.path             = .
macdeployqt.extra	 = $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
macdeployqt.path	 = BiblioteQ.app
postinstall.extra	 = cp -r BiblioteQ.app BiblioteQ.d/.
postinstall.path	 = BiblioteQ.d
preinstall.extra         = rm -fr BiblioteQ.d/BiblioteQ.app/*
preinstall.path          = BiblioteQ.d
sql.files		 = SQL/*.sql
sql.path		 = BiblioteQ.d

INSTALLS	= preinstall \
		  macdeployqt \
		  biblioteq \
                  conf \
                  data \
		  doc1 \
		  doc2 \
		  sql \
                  postinstall
