#ifndef _BIBLIOTEQ_SQLITE_MERGE_DATABASES_H_
#define _BIBLIOTEQ_SQLITE_MERGE_DATABASES_H_

#include "ui_biblioteq_merge_sqlite_databases.h"

class biblioteq;

class biblioteq_sqlite_merge_databases: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_sqlite_merge_databases(biblioteq *parent);

 private:
  Ui_mergeSQLiteDatabases m_ui;
  biblioteq *m_qmain;
  void changeEvent(QEvent *event);

 private slots:
  void setGlobalFonts(const QFont &font);
};

#endif
