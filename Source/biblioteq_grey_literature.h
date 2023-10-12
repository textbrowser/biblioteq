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

#ifndef _BIBLIOTEQ_GREY_LITERATURE_H_
#define _BIBLIOTEQ_GREY_LITERATURE_H_

#include "biblioteq_item.h"
#include "ui_biblioteq_greyliteratureinfo.h"

class biblioteq_grey_literature: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_grey_literature(biblioteq *parentArg,
			    const QString &oidArg,
			    const QModelIndex &index);
  ~biblioteq_grey_literature();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");

  void setPublicationDateFormat(const QString &dateFormat)
  {
    m_ui.date->setDisplayFormat(dateFormat);
  }

  void updateWindow(const int state);

 private:
  enum Columns
    {
      COMPRESSED_SIZE = 2,
      DESCRIPTION = 3,
      DIGEST = 1,
      FILE = 0,
      MYOID = 4
    };

  QPalette m_te_orig_pal;
  QString m_dt_orig_ss;
  QString m_engWindowTitle;
  Ui_biblioteq_greyliteratureinfo m_ui;
  bool m_duplicate;
  bool validateWidgets(void);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void createFile(const QByteArray &bytes,
		  const QByteArray &digest,
		  const QString &fileName) const;
  void highlightRequiredWidgets(void);
  void insertDatabase(void);
  void populateFiles(void);
  void prepareFavorites(void);
  void updateDatabase(void);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotAttachFiles(void);
  void slotCancel(void);
  void slotDeleteFiles(void);
  void slotExportFiles(void);
  void slotFilesDoubleClicked(QTableWidgetItem *item);
  void slotGo(void);
  void slotPrint(void);
  void slotPublicationDateEnabled(bool state);
  void slotQuery(void);
  void slotReset(void);
  void slotShowUsers(void);
};

#endif
