#ifndef _BIBLIOTEQ_MAIN_TABLE_H_
#define _BIBLIOTEQ_MAIN_TABLE_H_

/*
** -- Qt Includes --
*/

#include <QApplication>
#include <QHash>
#include <QHeaderView>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableWidgetItem>

class biblioteq_main_table: public QTableWidget
{
  Q_OBJECT

 public:
  biblioteq_main_table(QWidget *);
  QHash<QString, QString> friendlyStates(void) const;
  QStringList columnNames(void) const;
  int columnNumber(const QString &);
  void parseStates(const QHash<QString, QString> &);
  void recordColumnHidden
    (const QString &, const QString &, const int, const bool);
  void resetTable(const QString &, const QString &, const QString &);
  void setColumnNames(const QStringList &);

 private:
  QHash<QString, QList<int> > hiddenColumns;
  QVector<QString> m_columnHeaderIndexes;
  void setColumns(const QString &, const QString &, const QString &);
};

#endif
