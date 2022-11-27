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
                          -pedantic \
                          -std=c++17
QMAKE_DISTCLEAN += -r temp .qmake.cache .qmake.stash
QMAKE_EXTRA_TARGETS = purge
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13

ICON		= Icons/book.icns
INCLUDEPATH	+= /usr/local/include \
                   Source \
                   temp
LIBS		+= -L/usr/local/lib \
                   -framework Cocoa \
                   -lpq

exists(/usr/local/include/yaz) {
DEFINES         += BIBLIOTEQ_LINKED_WITH_YAZ
LIBS            += -lyaz
}

OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app

biblioteq.files		 = BiblioteQ.app/*
biblioteq.path		 = /Applications/BiblioteQ.d/BiblioteQ.app
conf.files		 = biblioteq.conf
conf.path		 = /Applications/BiblioteQ.d
data.files		 = Data/*
data.path		 = /Applications/BiblioteQ.d/Data
doc1.files		 = Documentation/*.html Documentation/*.pdf Documentation/*.txt Documentation/TO-DO
doc1.path		 = /Applications/BiblioteQ.d/Documentation
doc2.files		 = Documentation/Contributed/*.docx Documentation/Contributed/*.html Documentation/Contributed/*.pdf
doc2.path		 = /Applications/BiblioteQ.d/Documentation/Contributed
install_name_tool1.extra = install_name_tool -change /usr/local/Cellar/nettle/3.8.1/lib/libnettle.8.dylib @executable_path/../Frameworks/libnettle.8.dylib /Applications/BiblioteQ.d/BiblioteQ.app/Contents/Frameworks/libhogweed.6.dylib
install_name_tool1.path  = .
lrelease.extra           = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path            = .
lupdate.extra            = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
lupdate.path             = .
macdeployqt.extra	 = $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
macdeployqt.path	 = BiblioteQ.app
postinstall.extra	 = cp -r BiblioteQ.app /Applications/BiblioteQ.d/.
postinstall.path	 = /Applications/BiblioteQ.d
preinstall.extra         = rm -fr /Applications/BiblioteQ.d/BiblioteQ.app/*
preinstall.path          = /Applications/BiblioteQ.d
sql.files		 = SQL/*.sql
sql.path		 = /Applications/BiblioteQ.d
zzz.extra                = chown -Rh root:wheel /Applications/BiblioteQ.d
zzz.path                 = /Applications/BiblioteQ.d

INSTALLS	= preinstall \
		  macdeployqt \
		  biblioteq \
                  conf \
                  data \
		  doc1 \
		  doc2 \
		  sql \
                  postinstall \
                  install_name_tool1 \
                  zzz
