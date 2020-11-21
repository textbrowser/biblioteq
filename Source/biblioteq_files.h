#ifndef _BIBLIOTEQ_FILES_H_
#define _BIBLIOTEQ_FILES_H_

#include "ui_biblioteq_files.h"

class biblioteq;

class biblioteq_files: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_files(biblioteq *biblioteq);
  ~biblioteq_files();

 private:
  Ui_files m_ui;
  biblioteq *m_biblioteq;
  void changeEvent(QEvent *event);

 private slots:
  void slotClose(void);
  void slotRefresh(void);
};

#endif
