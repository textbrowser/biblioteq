#ifndef _BIBLIOTEQ_MAGAZINE_H_
#define _BIBLIOTEQ_MAGAZINE_H_

#include "biblioteq_item.h"
#include "ui_biblioteq_maginfo.h"
#include "ui_biblioteq_passwordPrompt.h"

#include <QNetworkReply>
#include <QPointer>

class biblioteq_generic_thread;

class biblioteq_magazine: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_magazine(biblioteq *parentArg,
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
  QNetworkAccessManager *m_sruManager;
  QPalette m_cb_orig_pal;
  QPalette m_te_orig_pal;
  QPalette m_white_pal;
  QPointer<biblioteq_generic_thread> m_thread;
  QPointer<biblioteq_item_working_dialog> m_sruWorking;
  QString m_dt_orig_ss;
  QString m_engWindowTitle;
  QString m_subType;
  Ui_magDialog ma;
  Ui_passwordDialog ui_p;
  bool m_duplicate;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void createFile(const QByteArray &digest,
		  const QByteArray &bytes,
		  const QString &fileName) const;
  void createSRUDialog(void);
  void populateFiles(void);

 protected slots:
  void setGlobalFonts(const QFont &font);
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotDeleteFiles(void);
  void slotExportFiles(void);
  void slotFilesDoubleClicked(QTableWidgetItem *item);
  void slotGo(void);
  void slotPopulateCopiesEditor(void);
  void slotPrint(void);
  void slotProxyAuthenticationRequired(const QNetworkProxy &proxy,
				       QAuthenticator *authenticator);
  void slotPublicationDateEnabled(bool state);
  void slotReset(void);
  void slotSRUCanceled(void);
  void slotSRUDownloadFinished(bool error);
  void slotSRUDownloadFinished(void);
  void slotSRUError(QNetworkReply::NetworkError error);
  void slotSRUQuery(void);
  void slotSRUQueryError(const QString &text);
  void slotSRUReadyRead(void);
  void slotSRUSslErrors(const QList<QSslError> &list);
  void slotSelectImage(void);
  void slotShowPDF(void);
  void slotShowUsers(void);
  void slotZ3950Query(void);
  void sruDownloadFinished(void);

 signals:
  void sruQueryError(const QString &text);
};

class biblioteq_journal: public biblioteq_magazine
{
  Q_OBJECT

 public:
  biblioteq_journal(biblioteq *parentArg,
		    const QString &oidArg,
		    const int rowArg);
  ~biblioteq_journal();
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

 private slots:
  void slotCancel(void);
};

#endif
