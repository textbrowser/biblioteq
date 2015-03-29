#ifndef _QTBOOK_CD_H_
#define _QTBOOK_CD_H_

/*
** -- Qt Includes --
*/

#include <QMenu>
#include <QDialog>
#include <QMainWindow>
#include <QStringList>
#include <QGraphicsScene>

/*
** -- Local Includes --
*/

#include "copy_editor.h"
#include "misc_functions.h"
#include "qtbook_item.h"
#include "ui_cdinfo.h"
#include "ui_tracks.h"

class copy_editor;

class qtbook_cd: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_cd(QMainWindow *,
	    const QString &, const int);
  ~qtbook_cd();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QAction *composer_action;
  QDialog *tracks_diag;
  QString engWindowTitle;
  Ui_cdDialog cd;
  Ui_tracksDialog trd;
  void changeEvent(QEvent *);
  void closeEvent(QCloseEvent *);

 private slots:
  void slotCancel(void);
  void slotCloseTracksBrowser(void);
  void slotComputeRuntime(void);
  void slotDeleteTrack(void);
  void slotGo(void);
  void slotInsertTrack(void);
  void slotPopulateCopiesEditor(void);
  void slotPopulateTracksBrowser(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotSaveTracks(void);
  void slotSelectImage(void);
  void slotShowUsers(void);
};

#endif
