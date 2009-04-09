#ifndef _QTBOOK_BOOK_H_
#define _QTBOOK_BOOK_H_

/*
** -- Qt Includes --
*/

#include <QHttp>
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
	      const QStringList &, const QString &, const int);
  ~qtbook_book();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  int requestid1; // Front Cover HTTP Get ID
  int requestid2; // Back Cover HTTP Get ID
  bool httpRequestAborted;
  QHttp *http1; // Front Cover
  QHttp *http2; // Back Cover
  QBuffer *imgbuffer1; // Front Cover Buffer
  QBuffer *imgbuffer2; // Back Cover Buffer
  QString cb_orig_ss;
  QString dt_orig_ss;
  QString engWindowTitle;
  QPalette te_orig_pal;
  QByteArray imgbytes1; // Front Cover Byte Array
  QByteArray imgbytes2; // Back Cover Byte Array
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
  void slotShowUsers(void);
  void slotSelectImage(void);
  void slotGenerateISBN(void);
  void slotDownloadImage(void);
  void slotConvertISBN10to13(void);
  void slotCancelImageDownload(void);
  void slotHttpRequestFinished(int, bool);
  void slotPopulateCopiesEditor(void);
  void slotUpdateDataReadProgress(int, int);
};

#endif
