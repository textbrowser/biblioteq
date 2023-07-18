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

#ifndef _BIBLIOTEQ_CD_H_
#define _BIBLIOTEQ_CD_H_

#include "biblioteq_item.h"
#include "ui_biblioteq_cdinfo.h"
#include "ui_biblioteq_tracks.h"

class biblioteq_cd: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_cd(biblioteq *parentArg,
	       const QString &oidArg,
	       const QModelIndex &index);
  ~biblioteq_cd();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");

  void setPublicationDateFormat(const QString &dateFormat)
  {
    cd.release_date->setDisplayFormat(dateFormat);
  }

  void updateWindow(const int);

 private:
  enum TracksColumns
    {
      ALBUM_NUMBER = 0,
      ARTIST = 4,
      COMPOSER = 5,
      TRACK_NUMBER = 1,
      TRACK_RUNTIME = 3,
      TRACK_TITLE = 2
    };

  QAction *m_composer_action;
  QDialog *m_tracks_diag;
  QString m_engWindowTitle;
  Ui_cdDialog cd;
  Ui_tracksDialog trd;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void prepareFavorites(void);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotCancel(void);
  void slotCloseTracksBrowser(void);
  void slotComputeRuntime(void);
  void slotDeleteTrack(void);
  void slotGo(void);
  void slotInsertTrack(void);
  void slotPopulateCopiesEditor(void);
  void slotPopulateTracksBrowser(void);
  void slotPrint(void);
  void slotPublicationDateEnabled(bool state);
  void slotQuery(void);
  void slotReset(void);
  void slotSaveTracks(void);
  void slotSelectImage(void);
  void slotShowUsers(void);
};

#endif
