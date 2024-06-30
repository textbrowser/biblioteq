include(biblioteq-source.pro)
dmg.commands = make install && \
               hdiutil create BiblioteQ.d.dmg -srcfolder BiblioteQ.d
purge.commands = find . -name '*~*' -exec rm -f {} \;

CONFIG		+= qt release warn_on

exists(/opt/homebrew/Cellar/libpq/16.3/lib/libpq.dylib) {
DEFINES         += BIBLIOTEQ_MACOS_LIBPQ_PATH="'\"/opt/homebrew/Cellar/libpq/16.3/lib/libpq.dylib\"'"
}

exists(/usr/local/Cellar/libpq/16.3/lib/libpq.dylib) {
DEFINES         += BIBLIOTEQ_MACOS_LIBPQ_PATH="'\"/usr/local/Cellar/libpq/16.3/lib/libpq.dylib\"'"
}

DEFINES		+= QT_DEPRECATED_WARNINGS
LANGUAGE	= C++
QT		+= gui network printsupport sql widgets
QT		-= webkit

QMAKE_CLEAN	+= BiblioteQ
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
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
QMAKE_DISTCLEAN += -r \
                   .qmake.cache \
                   .qmake.stash \
                   BiblioteQ \
                   BiblioteQ.d \
                   Temporary
QMAKE_EXTRA_TARGETS = dmg purge
QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0

ICON		= Icons/book.icns
INCLUDEPATH	+= Source
LIBS		+= -framework AppKit -framework Cocoa

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
doc1.files		 = Documentation/*.html Documentation/*.pdf \
                           Documentation/*.txt Documentation/TO-DO
doc1.path		 = BiblioteQ.d/Documentation
doc2.files		 = Documentation/Contributed/*.docx \
                           Documentation/Contributed/*.html \
                           Documentation/Contributed/*.pdf
doc2.path		 = BiblioteQ.d/Documentation/Contributed

exists(/opt/homebrew/Cellar/libpq/16.3/lib/libpq.dylib) {
installnametool1.extra   = install_name_tool -change /Applications/Postgres.app/Contents/Versions/14/lib/libpq.5.dylib /opt/homebrew/Cellar/libpq/16.3/lib/libpq.dylib ./BiblioteQ.d/BiblioteQ.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib
}

exists(/usr/local/Cellar/libpq/16.3/lib/libpq.dylib) {
installnametool1.extra   = install_name_tool -change /Applications/Postgres.app/Contents/Versions/14/lib/libpq.5.dylib /usr/local/Cellar/libpq/16.3/lib/libpq.dylib ./BiblioteQ.d/BiblioteQ.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib
}

installnametool1.path    = .
lrelease.extra           = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path            = .
lupdate.extra            = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
lupdate.path             = .
macdeployqt.extra	 = $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app \
                           -verbose=0 2>/dev/null ; echo;
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
                  postinstall \
                  installnametool1
