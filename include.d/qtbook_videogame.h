#ifndef _QTBOOK_VIDEOGAME_H_
#define _QTBOOK_VIDEOGAME_H_

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
#include "copy_editor.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"
#include "misc_functions.h"
#include "borrowers_editor.h"
#include "ui_videogameinfo.h"

class copy_editor;
class borrowers_editor;

class qtbook_videogame: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_videogame(QMainWindow *, const QStringList &,
		   const QStringList &, const QStringList &,
		   const QStringList &, const QStringList &, const QString &,
		   const int);
  ~qtbook_videogame();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void closeEvent(QCloseEvent *);
  void updateWindow(const int);

 private:
  Ui_vgDialog vg;

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
