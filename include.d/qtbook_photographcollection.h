#ifndef _QTBOOK_PHOTOGRAPHCOLLECTION_H_
#define _QTBOOK_PHOTOGRAPHCOLLECTION_H_

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

#include "qtbook_item.h"
#include "ui_photograph.h"
#include "misc_functions.h"
#include "ui_photographinfo.h"

class copy_editor;
class borrowers_editor;

class qtbook_photographcollection: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_photographcollection(QMainWindow *, const QString &,
			      const int);
  ~qtbook_photographcollection();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void duplicate(const QString &, const int);
  void updateWindow(const int);

 private:
  QDialog *photo_diag;
  QString engWindowTitle;
  Ui_pcDialog pc;
  Ui_photographDialog photo;
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);
  void showPhotographs(const int);

 private slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotAddItem(void);
  void slotSelectImage(void);
};

#endif
