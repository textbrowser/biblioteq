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
#include <QNetworkReply>
#include <QNetworkAccessManager>

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
  qtbook_book(QMainWindow *,
	      const QString &, const int);
  ~qtbook_book();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void duplicate(const QString &, const int);
  void updateWindow(const int);

 private:
  QString cb_orig_ss;
  QString dt_orig_ss;
  QString engWindowTitle;
  QPalette white_pal;
  QPalette te_orig_pal;
  generic_thread *thread;
  Ui_informationDialog id;
  qtbook_item_working_dialog *httpProgress;

 protected:
  void closeEvent(QCloseEvent *);

 private slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotReadyRead(void);
  void slotShowUsers(void);
  void slotSelectImage(void);
  void slotDownloadImage(void);
  void slotDownloadFinished(void);
  void slotConvertISBN10to13(void);
  void slotConvertISBN13to10(void);
  void slotCancelImageDownload(void);
  void slotDataTransferProgress(qint64, qint64);
  void slotPopulateCopiesEditor(void);
};

#endif
