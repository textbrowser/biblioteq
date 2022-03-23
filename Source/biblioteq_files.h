#ifndef _BIBLIOTEQ_FILES_H_
#define _BIBLIOTEQ_FILES_H_

#include <QPointer>

#include "ui_biblioteq_files.h"

class biblioteq;

class biblioteq_files: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_files(biblioteq *biblioteq);
  ~biblioteq_files();
  void reset(void);

 private:
  enum Columns
    {
     COMPRESSED_SIZE = 3,
     DESCRIPTION = 1,
     DIGEST = 2,
     FILE = 0,
     ITEM_TYPE = 5,
     MYOID = 6,
     PARENT_TITLE = 4
    };

  QPointer<biblioteq> m_biblioteq;
  Ui_files m_ui;
  void changeEvent(QEvent *event);

 private slots:
  void slotClose(void);
  void slotExport(void);
  void slotPagesChanged(int value);
  void slotRefresh(void);
};

#endif
