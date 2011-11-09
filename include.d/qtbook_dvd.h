#ifndef _QTBOOK_DVD_H_
#define _QTBOOK_DVD_H_

/*
** -- Qt Includes --
*/

#include <QMenu>
#include <QDialog>
#include <QMainWindow>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "ui_dvdinfo.h"
#include "copy_editor.h"
#include "qtbook_item.h"
#include "misc_functions.h"
#include "borrowers_editor.h"

class copy_editor;
class borrowers_editor;

class qtbook_dvd: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_dvd(QMainWindow *, const QString &, const int);
  ~qtbook_dvd();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void duplicate(const QString &, const int);
  void updateWindow(const int);

 private:
  QString engWindowTitle;
  Ui_dvdDialog dvd;

 protected:
  void closeEvent(QCloseEvent *);

 private slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotShowUsers(void);
  void slotSelectImage(void);
  void slotPopulateCopiesEditor(void);
};

#endif
