#ifndef _BIBLIOTEQ_CD_H_
#define _BIBLIOTEQ_CD_H_

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

#include "biblioteq_copy_editor.h"
#include "biblioteq_misc_functions.h"
#include "biblioteq_item.h"
#include "ui_cdinfo.h"
#include "ui_tracks.h"

class biblioteq_copy_editor;

class biblioteq_cd: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_cd(QMainWindow *,
	       const QString &, const int);
  ~biblioteq_cd();
  void duplicate(const QString &, const int);
  void insert(void);
  void modify(const int);
  void search(const QString & = "", const QString & = "");
  void updateWindow(const int);

 private:
  QAction *m_composer_action;
  QDialog *m_tracks_diag;
  QString m_engWindowTitle;
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
