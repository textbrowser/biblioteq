#ifndef _QTBOOK_DVD_H_
#define _QTBOOK_DVD_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QMainWindow>
#include <QMenu>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "copy_editor.h"
#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_dvdinfo.h"

class borrowers_editor;
class copy_editor;

class qtbook_dvd: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_dvd(QMainWindow *, const QString &, const int);
  ~qtbook_dvd();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QString engWindowTitle;
  Ui_dvdDialog dvd;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);

 private slots:
  void slotCancel(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotSelectImage(void);
  void slotShowUsers(void);
};

#endif
