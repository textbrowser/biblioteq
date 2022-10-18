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

#ifndef _BIBLIOTEQ_BATCH_ACTIVITIES_H_
#define _BIBLIOTEQ_BATCH_ACTIVITIES_H_

#include "ui_biblioteq_batch_activities_browser.h"

class biblioteq;

class biblioteq_batch_activities: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_batch_activities(biblioteq *parent);
  void show(QMainWindow *parent);

 private:
  enum BorrowTableColumns
    {
      CATEGORY_COLUMN = 0,
      DESCRIPTION_COLUMN = 4,
      COPY_IDENTIFIER_COLUMN = 2,
      IDENTIFIER_COLUMN = 1,
      RESULTS_COLUMN = 3
    };

  enum Pages
    {
      Borrow = 0
    };

  Ui_batchActivitiesBrowser m_ui;
  biblioteq *m_qmain;
  void borrow(void);
  void changeEvent(QEvent *event);

 private slots:
  void slotAddBorrowingRow(void);
  void slotClose(void);
  void slotDeleteBorrowingRow(void);
  void slotGo(void);
  void slotReset(void);
  void slotScanBorrowingTimerTimeout(void);
  void slotScannedBorrowing(void);
  void slotSetGlobalFonts(const QFont &font);
};

#endif
