#ifndef _BIBLIOTEQ_VIDEOGAME_H_
#define _BIBLIOTEQ_VIDEOGAME_H_

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

#include "biblioteq_copy_editor.h"
#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_borrowers.h"
#include "ui_videogameinfo.h"

class biblioteq_borrowers_editor;
class biblioteq_copy_editor;

class biblioteq_videogame: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_videogame(QMainWindow *, const QString &, const int);
  ~biblioteq_videogame();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QString m_engWindowTitle;
  Ui_vgDialog vg;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

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
