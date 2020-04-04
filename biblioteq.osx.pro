include(biblioteq-source.pro)

greaterThan(QT_MAJOR_VERSION, 4) {
cache()
}

purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= app_bundle qt release thread warn_on
DEFINES		+= QT_DEPRECATED_WARNINGS

LANGUAGE	= C++
QT		+= network sql
QT		-= webkit

greaterThan(QT_MAJOR_VERSION, 4) {
exists(/usr/local/include/poppler/qt5)
{
DEFINES +=      BIBLIOTEQ_LINKED_WITH_POPPLER
INCLUDEPATH     += /usr/local/include/poppler/qt5
LIBS    +=      -lpoppler-qt5
QT              += printsupport widgets
}
}

exists(/usr/local/include/poppler/cpp) {
DEFINES +=     BIBLIOTEQ_POPPLER_VERSION_DEFINED
INCLUDEPATH += /usr/local/include/poppler/cpp
} else {
message("The directory /usr/local/include/poppler/cpp does not exist. Poppler version information will not be available.")
}

QMAKE_CLEAN	+= BiblioteQ

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_CXX       = clang++
}

QMAKE_CXXFLAGS_RELEASE += -Wall -Wcast-align -Wcast-qual -Wextra \
			  -Wformat=2 -Woverloaded-virtual \
			  -Wpointer-arith -Wstrict-overflow=5 \
			  -Wstack-protector -fPIE -fstack-protector-all \
			  -fwrapv -mtune=generic

lessThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -pie
} else {
QMAKE_CXXFLAGS_RELEASE += -std=c++11
}

QMAKE_EXTRA_TARGETS = purge
QMAKE_DISTCLEAN += -r temp

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}
else {
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
}

ICON		= Icons/book.icns
INCLUDEPATH	+= Source temp /usr/local/include

greaterThan(QT_MAJOR_VERSION, 4) {
LIBS            += -framework Cocoa
}

LIBS		+= -lsqlite3 -L/usr/local/lib -lpq -lyaz
RESOURCES	= Icons/icons.qrc

greaterThan(QT_MAJOR_VERSION, 4) {
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
}

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app

biblioteq.path		= /Applications/BiblioteQ.d/BiblioteQ.app
biblioteq.files		= BiblioteQ.app/*
conf.path		= /Applications/BiblioteQ.d
conf.files		= biblioteq.conf
doc1.path		= /Applications/BiblioteQ.d/Documentation
doc1.files		= Documentation/*.pdf Documentation/*.txt Documentation/TO-DO
doc2.path		= /Applications/BiblioteQ.d/Documentation/Contributed
doc2.files		= Documentation/Contributed/*.docx Documentation/Contributed/*.pdf
install_name_tool1.path      = .
install_name_tool1.extra     = install_name_tool -change /usr/local/Cellar/poppler/0.87.0/lib/libpoppler.98.dylib @executable_path/../Frameworks/libpoppler.98.dylib /Applications/BiblioteQ.d/BiblioteQ.app/Contents/Frameworks/libpoppler-qt5.1.dylib
install_name_tool2.path      = .
install_name_tool2.extra     = install_name_tool -change /usr/local/Cellar/poppler/0.87.0/lib/libpoppler.98.dylib @executable_path/../Frameworks/libpoppler.98.dylib /Applications/BiblioteQ.d/BiblioteQ.app/Contents/Frameworks/libpoppler-qt5.1.dylib
lrelease.extra          = $$[QT_INSTALL_BINS]/lrelease biblioteq.osx.pro
lrelease.path           = .
lupdate.extra           = $$[QT_INSTALL_BINS]/lupdate biblioteq.osx.pro
lupdate.path            = .
macdeployqt.path	= BiblioteQ.app
macdeployqt.extra	= $$[QT_INSTALL_BINS]/macdeployqt ./BiblioteQ.app -verbose=0 2>/dev/null; echo;
preinstall.path         = /Applications/BiblioteQ.d
preinstall.extra        = rm -rf /Applications/BiblioteQ.d/BiblioteQ.app/*
postinstall.path	= /Applications/BiblioteQ.d
postinstall.extra	= cp -r BiblioteQ.app /Applications/BiblioteQ.d/.
sql.path		= /Applications/BiblioteQ.d
sql.files		= SQL/*.sql
translations.path	= /Applications/BiblioteQ.d/Translations
translations.files	= Translations/*.qm

INSTALLS	= preinstall \
		  macdeployqt \
		  biblioteq \
		  conf \
		  doc1 \
		  doc2 \
		  sql \
		  translations \
                  postinstall \
                  install_name_tool1 \
                  install_name_tool2
