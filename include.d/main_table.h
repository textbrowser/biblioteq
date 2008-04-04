#ifndef _MAIN_TABLE_H_
#define _MAIN_TABLE_H_

/*
** -- Qt Includes --
*/

#include <QHash>
#include <QFrame>
#include <QScrollBar>
#include <QHeaderView>
#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

class main_table: public QTableWidget
{
  Q_OBJECT

 public:
  main_table(QWidget *);
  void resetTable(const QString &);
  void recordColumnHidden(const QString &, const int, const bool);
  void clearHiddenColumnsRecord(void);

 private:
  QHash<QString, QList<int> > hiddenColumns;
  void setColumns(const QString &);
};

#endif
