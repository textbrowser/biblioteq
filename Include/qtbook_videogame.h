#ifndef _QTBOOK_VIDEOGAME_H_
#define _QTBOOK_VIDEOGAME_H_

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
#include "ui_borrowers.h"
#include "ui_videogameinfo.h"

class borrowers_editor;
class copy_editor;

class qtbook_videogame: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_videogame(QMainWindow *, const QString &,
		   const int);
  ~qtbook_videogame();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QString engWindowTitle;
  Ui_vgDialog vg;
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
