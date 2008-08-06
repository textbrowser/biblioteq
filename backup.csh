#!/bin/csh -x

set BACKUP_DIR = /love/scsi/backup.d/biblioteq.d

if ( ! -d $BACKUP_DIR ) then
    mkdir $BACKUP_DIR
endif

if ( ! -d $BACKUP_DIR/include.d ) then
    mkdir $BACKUP_DIR/include.d
endif

if ( ! -d $BACKUP_DIR/icons.d ) them
    mkdir $BACKUP_DIR/icons.d
endif

set osname = "`uname -s`"

if ( "$osname" == "Darwin" ) then
    qmake -o Makefile biblioteq.osx.pro
else if ( "$osname" == "FreeBSD" ) then
    if ( "`which qmake-qt4`" != "" ) then
	qmake-qt4 -o Makefile biblioteq.pro
    else
	qmake -o Makefile biblioteq.pro
    endif
else
    qmake -o Makefile biblioteq.pro
endif

make clean
make distclean

cp -p *.cc $BACKUP_DIR/.
cp -pfR icons.d/* $BACKUP_DIR/icons.d/.
cp -p include.d/*.h $BACKUP_DIR/include.d/.
cp -p biblioteq*.pro $BACKUP_DIR/.
cp -p biblioteq.conf $BACKUP_DIR/.
cp -p backup.csh $BACKUP_DIR/.
cp -p *.ui $BACKUP_DIR/.
cp -p *.sql $BACKUP_DIR/.
cp -p INSTALL $BACKUP_DIR/.
cp -p INSTALL.freebsd $BACKUP_DIR/.
cp -p TO-DO $BACKUP_DIR/.
rm -rf $BACKUP_DIR/icons.d/16x16/.svn
rm -rf $BACKUP_DIR/icons.d/32x32/.svn
