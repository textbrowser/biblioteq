cache()
include(biblioteq-source.pro)

purge.commands = del /Q *~ && del /Q *\\*~

CONFIG += qt release warn_on windows
DEFINES += BIBLIOTEQ_LINKED_WITH_YAZ
LANGUAGE = C++
QT += gui network printsupport sql widgets
QT -= webkit

QMAKE_CLEAN += BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wextra \
                          -Wformat=2 \
			  -Wno-deprecated-declarations \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash debug Temporary
QMAKE_EXTRA_TARGETS = purge

ICON = Icons\\book.png
INCLUDEPATH += Distributions\\Windows\\Include.64 Source
LIBS += -L"." \
        -L"Distributions\\Windows\\Libraries.64" -lyaz5
RC_FILE	= Distributions\\Windows\\biblioteq.win.rc
PROJECTNAME = BiblioteQ
TARGET = BiblioteQ
TEMPLATE = app

biblioteq.files = biblioteq.conf
biblioteq.path = release\\.
biblioteq_bat.files = Distributions\\biblioteq-darkmode.bat
biblioteq_bat.path = release\\.
data.files = Data\\*
data.path = release\\Data\\.
documentation.files = Documentation\\*.html \
                      Documentation\\*.pdf \
                      Documentation\\Contributed \
                      Documentation\\REMINDERS
documentation.path = release\\Documentation\\.
libraries.files = Distributions\\Windows\\Libraries.64\\*.dll \
                  Distributions\\Windows\\Libraries.64\\*.exe
libraries.path = release\\.
plugins.files = $$[QT_INSTALL_PLUGINS]\\*
plugins.path = release\\plugins\\.
pluginspurge.extra = del /Q /S *.debug
pluginspurge.path = release\\plugins\\.
qt.files = Distributions\\Windows\\qt.conf
qt.path = release\\.
qtlibraries.files = $$[QT_INSTALL_BINS]\\Qt*Concurrent.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Core.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Gui.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Multimedia.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Network.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Pdf.dll \
                    $$[QT_INSTALL_BINS]\\Qt*PdfWidgets.dll \
                    $$[QT_INSTALL_BINS]\\Qt*PrintSupport.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Sql.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Widgets.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Xml.dll \
                    $$[QT_INSTALL_BINS]\\libgcc_s_seh-1.dll \
                    $$[QT_INSTALL_BINS]\\libstdc++-*.dll \
                    $$[QT_INSTALL_BINS]\\libwinpthread-*.dll
qtlibraries.path = release\\.
sql1.files = SQL\\README*
sql1.path = release\\SQL\\.
sql2.files = SQL\\*.sql
sql2.path = release\\SQL\\.

INSTALLS = biblioteq \
           biblioteq_bat \
           data \
           documentation \
           libraries \
           plugins \
           pluginspurge \
           qt \
           qtlibraries \
           sql1 \
           sql2
