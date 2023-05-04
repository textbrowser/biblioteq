/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
		     const QModelIndex &index,
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
  enum Columns
    {
      COMPRESSED_SIZE = 2,
      DESCRIPTION = 3,
      DIGEST = 1,
      FILE = 0,
      MYOID = 4
    };

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
  void resetQueryHighlights(void);

 protected slots:
  void setGlobalFonts(const QFont &font);
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotDeleteFiles(void);
  void slotExportFiles(void);
  void slotFilesDoubleClicked(QTableWidgetItem *item);
  void slotGo(void);
  void slotParseMarcTags(void);
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
		    const QModelIndex &index);
  ~biblioteq_journal();
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

 private slots:
  void slotCancel(void);
};

#endif
