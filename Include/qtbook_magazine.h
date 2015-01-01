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

#include "ui_maginfo.h"
#include "copy_editor.h"
#include "qtbook_item.h"
#include "ui_borrowers.h"
#include "z3950results.h"
#include "generic_thread.h"
#include "misc_functions.h"

class copy_editor;
class generic_thread;
class borrowers_editor;

class qtbook_magazine: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_magazine(QMainWindow *, const QString &,
		  const int, const QString &);
  ~qtbook_magazine();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void duplicate(const QString &, const int);
  void updateWindow(const int);
  void populateDisplayAfterSRU(const QByteArray &);
  void populateDisplayAfterZ3950(const QStringList &,
				 const QString &);
  Ui_magDialog dialog(void) const;

 protected:
  QString subType;
  QString dt_orig_ss;
  QString engWindowTitle;
  QPalette white_pal;
  QPalette cb_orig_pal;
  QPalette te_orig_pal;
  QByteArray m_sruResults;
  Ui_magDialog ma;
  generic_thread *thread;
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);

 protected slots:
  void slotGo(void);
  void slotPrint(void);
  void slotReset(void);
  void slotCancel(void);
  void slotSRUQuery(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
  void slotSelectImage(void);
  void slotSRUReadyRead(void);
  void slotSRUDownloadFinished(void);
  void slotPopulateCopiesEditor(void);
};

class qtbook_journal: public qtbook_magazine
{
  Q_OBJECT

 public:
  qtbook_journal(QMainWindow *, const QString &,
		 const int);
  ~qtbook_journal();
  void closeEvent(QCloseEvent *);
  void changeEvent(QEvent *);

 private slots:
  void slotCancel(void);
};

#endif
