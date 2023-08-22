cache()
include(biblioteq-source.pro)

purge.commands = del /Q *~ && del /Q *\\*~

CONFIG		+= qt release thread warn_on windows
DEFINES         += BIBLIOTEQ_LINKED_WITH_YAZ
LANGUAGE	= C++
QT		+= network sql printsupport widgets
QT		-= webkit

QMAKE_CLEAN	+= BiblioteQ.exe
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wextra \
                          -Wformat=2 \
			  -Wno-deprecated-declarations \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstrict-overflow=5 \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
QMAKE_DISTCLEAN += .qmake.cache .qmake.stash debug temp
QMAKE_EXTRA_TARGETS = purge

ICON		= Icons\\book.png
INCLUDEPATH	+= Include.win32 \
                   Source \
                   temp
LIBS		+= -L"." \
                   -L"Libraries.win32\\yaz" \
                   -lyaz5
RC_FILE		= biblioteq.win.rc
PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ
TEMPLATE        = app

biblioteq.files = biblioteq.conf
biblioteq.path = release\\.
biblioteq_bat.files = biblioteq-darkmode.bat
biblioteq_bat.path = release\\.
data.files = Data\\*
data.path = release\\Data\\.
documentation.files = Documentation\\*.html \
                      Documentation\\*.pdf \
                      Documentation\\Contributed \
                      Documentation\\TO-DO
documentation.path = release\\Documentation\\.
libraries.files = Libraries.win32\\miscellaneous\\*.dll \
                  Libraries.win32\\postgresql\\*.dll \
                  Libraries.win32\\postgresql\\*.manifest \
                  Libraries.win32\\yaz\\*.dll \
                  Libraries.win32\\yaz\\*.exe \
                  Libraries.win64\\*.exe
libraries.path = release\\.
plugins1.files = $$[QT_INSTALL_PLUGINS]\\*
plugins1.path = release\\plugins\\.
plugins2.files = $$[QT_INSTALL_PLUGINS]\\gamepads\\xinputgamepad.dll
plugins2.path = release\\plugins\\gamepads\\.
plugins3.files = $$[QT_INSTALL_PLUGINS]\\platforms\\qdirect2d.dll
plugins3.path = release\\plugins\\platforms\\.
plugins4.files = $$[QT_INSTALL_PLUGINS]\\renderplugins\\scene2d.dll
plugins4.path = release\\plugins\\renderplugins\\.
pluginspurge1.extra = del /Q /S *.debug
pluginspurge1.path = release\\plugins\\.
pluginspurge2.extra = del /Q /S *d.dll
pluginspurge2.path = release\\plugins\\.
qt.files = Qt\\qt.conf
qt.path = release\\.
qtlibraries.files = $$[QT_INSTALL_BINS]\\Qt6Concurrent.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Core.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Gui.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Multimedia.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Network.dll \
                    $$[QT_INSTALL_BINS]\\Qt6PrintSupport.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Sql.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Widgets.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Xml.dll \
                    $$[QT_INSTALL_BINS]\\libgcc_s_dw2-1.dll \
                    $$[QT_INSTALL_BINS]\\libstdc++-6.dll \
                    $$[QT_INSTALL_BINS]\\libwinpthread-1.dll
qtlibraries.path = release\\.
sql1.files = SQL\\README*
sql1.path = release\\SQL\\.
sql2.files = SQL\\*.sql
sql2.path = release\\SQL\\.

INSTALLS = biblioteq \
           biblioteq_bat \
           data \
           plugins1 \
           pluginspurge1 \
           pluginspurge2 \
           libraries \
           documentation \
           plugins2 \
           plugins3 \
           plugins4 \
           qt \
           qtlibraries \
           sql1 \
           sql2
