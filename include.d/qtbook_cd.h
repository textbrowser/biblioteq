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

#include "ui_cdinfo.h"
#include "ui_tracks.h"
#include "copy_editor.h"
#include "qtbook_item.h"
#include "misc_functions.h"

class copy_editor;

class qtbook_cd: public QMainWindow, public qtbook_item
{
  Q_OBJECT

 public:
  qtbook_cd(QMainWindow *,
	    const QString &, const int);
  ~qtbook_cd();
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void duplicate(const QString &, const int);
  void updateWindow(const int);

 private:
  QDialog *tracks_diag;
  QString engWindowTitle;
  Ui_cdDialog cd;
  Ui_tracksDialog trd;

 protected:
  void closeEvent(QCloseEvent *);

 private slots:
  void slotGo(void);
  void slotPrint(void);
  void slotQuery(void);
  void slotReset(void);
  void slotCancel(void);
  void slotShowUsers(void);
  void slotSaveTracks(void);
  void slotDeleteTrack(void);
  void slotInsertTrack(void);
  void slotSelectImage(void);
  void slotComputeRuntime(void);
  void slotCloseTracksBrowser(void);
  void slotPopulateCopiesEditor(void);
  void slotPopulateTracksBrowser(void);
};

#endif
