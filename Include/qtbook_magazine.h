#ifndef _QTBOOK_MAGAZINE_H_
#define _QTBOOK_MAGAZINE_H_

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
#include "generic_thread.h"
#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"
#include "ui_maginfo.h"
#include "z3950results.h"

class borrowers_editor;
class copy_editor;
class generic_thread;

class qtbook_magazine: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_magazine(QMainWindow *, const QString &,
		  const int, const QString &);
  ~qtbook_magazine();
  Ui_magDialog dialog(void) const;
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void populateDisplayAfterSRU(const QByteArray &);
  void populateDisplayAfterZ3950(const QStringList &, const QString &);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 protected:
  QByteArray m_sruResults;
  QPalette cb_orig_pal;
  QPalette te_orig_pal;
  QPalette white_pal;
  QString dt_orig_ss;
  QString engWindowTitle;
  QString subType;
  Ui_magDialog ma;
  generic_thread *thread;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);

 protected slots:
  void slotCancel(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotReset(void);
  void slotSRUDownloadFinished(void);
  void slotSRUQuery(void);
  void slotSRUReadyRead(void);
  void slotSelectImage(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
};

class qtbook_journal: public qtbook_magazine
{
  Q_OBJECT

 public:
  qtbook_journal(QMainWindow *, const QString &,
		 const int);
  ~qtbook_journal();
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);

 private slots:
  void slotCancel(void);
};

#endif
