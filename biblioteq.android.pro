cache()
include(biblioteq-source.pro)

purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release thread warn_on
DEFINES		+= BIBLIOTEQ_CONFIGFILE="'\"biblioteq.conf\"'"
LANGUAGE	= C++
QMAKE_CLEAN	+= BiblioteQ
QT              += network printsupport sql widgets
TEMPLATE	= app

QMAKE_CXXFLAGS_RELEASE += -Wall \
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

INCLUDEPATH	+= Android Source temp
LIBS		+=
SOURCES         += Android/sqlite3/sqlite3.c

PROJECTNAME	= BiblioteQ
TARGET		= BiblioteQ

DISTFILES += android/AndroidManifest.xml \
             android/build.gradle \
             android/gradle.properties \
             android/gradle/wrapper/gradle-wrapper.jar \
             android/gradle/wrapper/gradle-wrapper.properties \
             android/gradlew \
             android/gradlew.bat \
             android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}
