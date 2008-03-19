#ifndef _QTBOOK_MAGAZINE_H_
#define _QTBOOK_MAGAZINE_H_

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
#include "locresults.h"
#include "ui_maginfo.h"
#include "copy_editor.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "borrowers_editor.h"

class copy_editor;
class generic_thread;
class borrowers_editor;

class qtbook_magazine: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_magazine(QMainWindow *, const QStringList &, const QStringList &,
		  const QStringList &, const QStringList &, const QString &,
		  const int);
  ~qtbook_magazine();
  bool isBusy(void);
  void insert(void);
  void modify(const int);
  void search(void);
  void updateWindow(const int);
  void populateDisplayAfterLOC(const QStringList &);
  virtual void closeEvent(QCloseEvent *);

 protected:
  QString subType;
  QPalette dt_orig_pal;
  QPalette te_orig_pal;
  Ui_magDialog ma;
  generic_thread *thread;

 protected slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotShowUsers(void);
  void slotPopulateCopiesEditor(void);
};

class qtbook_journal: public qtbook_magazine
{
  Q_OBJECT

 public:
  qtbook_journal(QMainWindow *, const QStringList &, const QStringList &,
		 const QStringList &, const QStringList &, const QString &,
		 const int);
  ~qtbook_journal();
  virtual void closeEvent(QCloseEvent *);

 private slots:
  void slotCancel(void);
};

#endif
