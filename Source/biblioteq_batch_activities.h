#ifndef _BIBLIOTEQ_BATCH_ACTIVITIES_H_
#define _BIBLIOTEQ_BATCH_ACTIVITIES_H_

#include "ui_biblioteq_batch_activities_browser.h"

class QProgressDialog;
class biblioteq;

class biblioteq_batch_activities: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_batch_activities(biblioteq *parent);
  void show(QMainWindow *parent);

 private:
  Ui_batchActivitiesBrowser m_ui;
  biblioteq *m_qmain;
  void changeEvent(QEvent *event);

 private slots:
  void slotAddBorrowingRow(void);
  void slotClose(void);
  void slotDeleteBorrowingRow(void);
  void slotGo(void);
  void slotReset(void);
  void slotSetGlobalFonts(const QFont &font);
};

#endif
