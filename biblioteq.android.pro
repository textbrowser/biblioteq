cache()
include(biblioteq-source.pro)

CONFIG	    += qt release warn_on
LANGUAGE    = C++
QMAKE_CLEAN += BiblioteQ
QT          += core gui network printsupport sql svg widgets
TEMPLATE    = app

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
                          -fPIC \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
QMAKE_DISTCLEAN        += -r .qmake.cache .qmake.stash Temporary
QMAKE_EXTRA_TARGETS    = purge

INCLUDEPATH += Source
LIBS	    +=

PROJECTNAME = BiblioteQ
TARGET	    = BiblioteQ

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android

DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/gradle.properties \
             Android/gradle/wrapper/gradle-wrapper.jar \
             Android/gradle/wrapper/gradle-wrapper.properties \
             Android/gradlew \
             Android/gradlew.bat \
             Android/res/values/libs.xml

deployment.files = Data/*.csv \
                   Data/*.sqlite \
                   Documentation/*.html \
                   Documentation/Contributed/French/*.html \
                   biblioteq.conf
deployment.path  = /assets

INSTALLS += deployment

android: include(/home/saturn/Android/Sdk/android_openssl/openssl.pri)
