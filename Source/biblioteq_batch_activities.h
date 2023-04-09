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

#include <QApplication>
#ifdef BIBLIOTEQ_AUDIO_SUPPORTED
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QAudioOutput>
#endif
#include <QMediaPlayer>
#endif

#include "ui_biblioteq_batch_activities_browser.h"

class biblioteq;

class biblioteq_batch_activities: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_batch_activities(biblioteq *parent);
  void reset(void);
  void show(QMainWindow *parent, const bool center = true);

 private:
  enum class BorrowTableColumns
    {
      CATEGORY_COLUMN = 0,
      COPY_IDENTIFIER_COLUMN = 2,
      FIELD_COLUMN = 4,
      IDENTIFIER_COLUMN = 1,
      RESULTS_COLUMN = 3
    };

  enum class DiscoverTableColumns
    {
      CATEGORY_COLUMN = 1,
      IDENTIFIER_COLUMN = 0,
      TITLE_COLUMN = 2
    };

  enum class Pages
    {
      Borrow = 0,
      Discover = 1,
      Return
    };

  enum class ReturnTableColumns
    {
      BORROWER_COLUMN = 1,
      CATEGORY_COLUMN = 2,
      IDENTIFIER_COLUMN = 0,
      TITLE_COLUMN = 3
    };

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#ifdef BIBLIOTEQ_AUDIO_SUPPORTED
  QScopedPointer<QAudioOutput> m_audioOutput;
#endif
#endif
  Ui_batchActivitiesBrowser m_ui;
  biblioteq *m_qmain;
  static QColor s_notSoOkColor;
  static QColor s_okColor;
  void borrow(void);
  void changeEvent(QEvent *event);
  void discover(void);
  void play(const QString &file);

 private slots:
  void slotAddBorrowingRow(void);
  void slotAudioEnabled(void);
  void slotBorrowItemChanged(QTableWidgetItem *item);
  void slotClose(void);
  void slotDeleteBorrowingRow(void);
  void slotExportMissing(void);
  void slotGo(void);
  void slotListDiscoveredItems(void);
  void slotListMembersReservedItems(void);
#ifdef BIBLIOTEQ_AUDIO_SUPPORTED
  void slotMediaError(QMediaPlayer::Error error);
  void slotMediaError(QMediaPlayer::Error error, const QString &errorString);
  void slotMediaStatusChanged(QMediaPlayer::MediaStatus status);
#endif
  void slotReset(void);
  void slotScanBorrowingTimerTimeout(void);
  void slotScanDiscoverTimerTimeout(void);
  void slotScanReturnTimerTimeout(void);
  void slotScannedBorrowing(void);
  void slotScannedDiscover(void);
  void slotScannedReturn(void);
  void slotSetGlobalFonts(const QFont &font);

 signals:
  void listMembersReservedItems(const QString &id);
};

#endif
