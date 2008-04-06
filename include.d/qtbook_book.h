#ifndef _QTBOOK_BOOK_H_
#define _QTBOOK_BOOK_H_

/*
** -- Qt Includes --
*/

#include <QMenu>
#include <QBuffer>
#include <QDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "copy_editor.h"
#include "qtbook_item.h"
#include "ui_bookinfo.h"
#include "ui_borrowers.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "borrowers_editor.h"

class copy_editor;
class generic_thread;
class borrowers_editor;

class qtbook_book: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_book(QMainWindow *, const QStringList &, const QStringList &,
	      const QStringList &, const QStringList &, const QString &,
	      const int);
  ~qtbook_book();
  bool isBusy(void);
  void insert(void);
  void modify(const int);
  void search(void);
  void updateWindow(const int);
  virtual void closeEvent(QCloseEvent *);

 private:
  QPalette cb_orig_pal;
  QPalette dt_orig_pal;
  QPalette te_orig_pal;
  generic_thread *thread;
  Ui_informationDialog id;

 private slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotShowUsers(void);
  void slotSelectImage(void);
  void slotConvertISBN10to13(void);
  void slotPopulateCopiesEditor(void);
};

#endif
