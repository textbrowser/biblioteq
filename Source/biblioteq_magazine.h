#ifndef _BIBLIOTEQ_MAGAZINE_H_
#define _BIBLIOTEQ_MAGAZINE_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#if QT_VERSION < 0x050000
#include <QHttp>
#endif
#include <QMainWindow>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QPointer>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "biblioteq_copy_editor.h"
#include "biblioteq_generic_thread.h"
#include "biblioteq_item.h"
#include "biblioteq_misc_functions.h"
#include "biblioteq_z3950results.h"
#include "ui_biblioteq_borrowers.h"
#include "ui_biblioteq_maginfo.h"
#include "ui_biblioteq_passwordPrompt.h"

class biblioteq_borrowers_editor;
class biblioteq_copy_editor;
class biblioteq_generic_thread;

class biblioteq_magazine: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_magazine(QMainWindow *parentArg,
		     const QString &oidArg,
		     const int rowArg,
		     const QString &subTypeArg);
  virtual ~biblioteq_magazine();
  Ui_magDialog dialog(void) const;
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int);
  void populateDisplayAfterSRU(const QByteArray &data);
  void populateDisplayAfterZ3950(const QStringList &list,
				 const QString &recordSyntax);

  void setPublicationDateFormat(const QString &dateFormat)
  {
    ma.publication_date->setDisplayFormat(dateFormat);
  }

  void search(const QString &field = "", const QString &value = "");
  void updateWindow(const int state);

 protected:
  QByteArray m_sruResults;
  QDialog *m_proxyDialog;
#if QT_VERSION < 0x050000
  QHttp *m_sruHttp;
#endif
  QNetworkAccessManager *m_sruManager;
  QPalette m_cb_orig_pal;
  QPalette m_te_orig_pal;
  QPalette m_white_pal;
  QPointer<biblioteq_generic_thread> m_thread;
  QString m_dt_orig_ss;
  QString m_engWindowTitle;
  QString m_subType;
  Ui_magDialog ma;
  Ui_passwordDialog ui_p;
  biblioteq_item_working_dialog *m_sruWorking;
  bool m_duplicate;
  bool useHttp(void) const;
  void changeEvent(QEvent *event);
  void createFile(const QByteArray &digest,
		  const QByteArray &bytes,
		  const QString &fileName) const;
  void closeEvent(QCloseEvent *event);
  void populateFiles(void);
  void sruDownloadFinished(void);

 protected slots:
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotDeleteFiles(void);
  void slotEditFileDescription(QTableWidgetItem *item);
  void slotExportFiles(void);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotProxyAuthenticationRequired
    (const QNetworkProxy &proxy, QAuthenticator *authenticator);
  void slotReset(void);
  void slotSRUDownloadFinished(bool error);
  void slotSRUDownloadFinished(void);
  void slotSRUQuery(void);
#if QT_VERSION < 0x050000
  void slotSRUReadyRead(const QHttpResponseHeader &resp);
#endif
  void slotSRUReadyRead(void);
  void slotSelectImage(void);
  void slotShowPDF(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
};

class biblioteq_journal: public biblioteq_magazine
{
  Q_OBJECT

 public:
  biblioteq_journal(QMainWindow *parentArg,
		    const QString &oidArg,
		    const int rowArg);
  ~biblioteq_journal();
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

 private slots:
  void slotCancel(void);
};

#endif
