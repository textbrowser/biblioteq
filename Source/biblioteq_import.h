#ifndef _BIBLIOTEQ_IMPORT_H_
#define _BIBLIOTEQ_IMPORT_H_

#include "ui_biblioteq_import.h"

class biblioteq;

class biblioteq_import: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_import(biblioteq *parent);
  void closeEvent(QCloseEvent *event);
  void show(QMainWindow *parent);

 private:
  Ui_importBrowser m_ui;
  biblioteq *qmain;
  void changeEvent(QEvent *event);

 private slots:
  void slotAddBookRow(void);
  void slotDeleteBookRow(void);
};

#endif
