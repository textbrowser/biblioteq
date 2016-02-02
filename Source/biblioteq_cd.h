#ifndef _BIBLIOTEQ_CD_H_
#define _BIBLIOTEQ_CD_H_

/*
** -- Qt Includes --
*/

#include <QDialog>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QStringList>

/*
** -- Local Includes --
*/

#include "biblioteq_copy_editor.h"
#include "biblioteq_misc_functions.h"
#include "biblioteq_item.h"
#include "ui_biblioteq_cdinfo.h"
#include "ui_biblioteq_tracks.h"

class biblioteq_copy_editor;

class biblioteq_cd: public QMainWindow, public biblioteq_item
{
  Q_OBJECT

 public:
  biblioteq_cd(QMainWindow *parentArg,
	       const QString &oidArg,
	       const int rowArg);
  ~biblioteq_cd();
  void duplicate(const QString &p_oid, const int state);
  void insert(void);
  void modify(const int state);
  void search(const QString &field = "", const QString &value = "");
  void updateWindow(const int);

 private:
  QAction *m_composer_action;
  QDialog *m_tracks_diag;
  QString m_engWindowTitle;
  Ui_cdDialog cd;
  Ui_tracksDialog trd;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

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
