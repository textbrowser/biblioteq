#ifndef _QTBOOK_BOOK_H_
#define _QTBOOK_BOOK_H_

/*
** -- Qt Includes --
*/

#include <QBuffer>
#include <QDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "copy_editor_book.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_bookinfo.h"
#include "ui_borrowers.h"
#include "ui_passwordPrompt.h"

class borrowers_editor;
class copy_editor_book;
class generic_thread;

class qtbook_book: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_book(QMainWindow *,
	      const QString &, const int);
  ~qtbook_book();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QByteArray m_sruResults;
  QDialog *m_proxyDialog;
  QPalette te_orig_pal;
  QPalette white_pal;
  QString cb_orig_ss;
  QString dt_orig_ss;
  QString engWindowTitle;
  Ui_informationDialog id;
  Ui_passwordDialog ui_p;
  generic_thread *thread;
  qtbook_item_working_dialog *httpProgress;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);

 private slots:
  void slotCancel(void);
  void slotCancelImageDownload(void);
  void slotConvertISBN10to13(void);
  void slotConvertISBN13to10(void);
  void slotDataTransferProgress(qint64, qint64);
  void slotDownloadFinished(void);
  void slotDownloadImage(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotProxyAuthenticationRequired
    (const QNetworkProxy &, QAuthenticator *);
  void slotReadyRead(void);
  void slotReset(void);
  void slotSRUDownloadFinished(void);
  void slotSRUError(QNetworkReply::NetworkError error);
  void slotSRUQuery(void);
  void slotSRUReadyRead(void);
  void slotSRUSslErrors(const QList<QSslError> &list);
  void slotSelectImage(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
};

#endif
