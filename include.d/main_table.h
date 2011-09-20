#ifndef _MAIN_TABLE_H_
#define _MAIN_TABLE_H_

/*
** -- Qt Includes --
*/

#include <QHash>
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
  void resetTable(const QString &, const QString &, const QString &);
  void parseStates(const QHash<QString, QString> &);
  void recordColumnHidden(const QString &, const QString &, const int,
			  const bool);
  QHash<QString, QString> friendlyStates(void) const;

 private:
  QHash<QString, QList<int> > hiddenColumns;
  void setColumns(const QString &, const QString &, const QString &);
};

#endif
