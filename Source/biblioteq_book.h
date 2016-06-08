#ifndef _BIBLIOTEQ_BOOK_H_
#define _BIBLIOTEQ_BOOK_H_

/*
** -- Qt Includes --
*/

#include <QBuffer>
#include <QDialog>
#include <QFileDialog>
#if QT_VERSION < 0x050000
#include <QHttp>
#endif
#include <QMainWindow>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "biblioteq_copy_editor_book.h"
#include "biblioteq_generic_thread.h"
#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "ui_biblioteq_bookinfo.h"
#include "ui_biblioteq_borrowers.h"
#include "ui_biblioteq_passwordPrompt.h"

class biblioteq_borrowers_editor;
class biblioteq_copy_editor_book;
class biblioteq_generic_thread;

class biblioteq_book: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_book(QMainWindow *parentArg,
		 const QString &oidArg,
		 const int rowArg);
  ~biblioteq_book();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");
  void updateWindow(const int);

 private:
  QBuffer m_imageBuffer;
  QByteArray m_sruResults;
  QDialog *m_proxyDialog;
#if QT_VERSION < 0x050000
  QHttp *m_imageHttp;
  QHttp *m_sruHttp;
#endif
  QNetworkAccessManager *m_imageManager;
  QNetworkAccessManager *m_sruManager;
  QPalette m_te_orig_pal;
  QPalette m_white_pal;
  QString m_cb_orig_ss;
  QString m_dt_orig_ss;
  QString m_engWindowTitle;
  Ui_informationDialog id;
  Ui_passwordDialog ui_p;
  biblioteq_generic_thread *m_thread;
  biblioteq_item_working_dialog *m_httpProgress;
  biblioteq_item_working_dialog *m_sruWorking;
  bool useHttp(void) const;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void createFile(const QByteArray &digest,
		  const QByteArray &bytes,
		  const QString &fileName) const;
  void downloadFinished(void);
  void populateFiles(void);
  void sruDownloadFinished(void);

 private slots:
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotCancelImageDownload(void);
  void slotConvertISBN10to13(void);
  void slotConvertISBN13to10(void);
  void slotDataTransferProgress(qint64 bytesread, qint64 totalbytes);
  void slotDeleteFiles(void);
  void slotDownloadFinished(bool error);
  void slotDownloadFinished(void);
  void slotDownloadImage(void);
  void slotEditFileDescription(QTableWidgetItem *item);
  void slotExportFiles(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotPrintAuthorTitleDewey(void);
  void slotPrintCallDewey(void);
  void slotProxyAuthenticationRequired
    (const QNetworkProxy &proxy, QAuthenticator *authenticator);
#if QT_VERSION < 0x050000
  void slotReadyRead(const QHttpResponseHeader &resp);
#endif
  void slotReadyRead(void);
  void slotReset(void);
  void slotSRUDownloadFinished(bool error);
  void slotSRUDownloadFinished(void);
  void slotSRUError(QNetworkReply::NetworkError error);
  void slotSRUQuery(void);
#if QT_VERSION < 0x050000
  void slotSRUReadyRead(const QHttpResponseHeader &resp);
#endif
  void slotSRUReadyRead(void);
  void slotSRUSslErrors(const QList<QSslError> &list);
  void slotSelectImage(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
};

#endif
