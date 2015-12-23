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
  biblioteq_main_table(QWidget *parent);
  QHash<QString, QString> friendlyStates(void) const;
  QStringList columnNames(void) const;
  int columnNumber(const QString &name) const;
  void parseStates(const QHash<QString, QString> &states);
  void recordColumnHidden(const QString &username,
			  const QString &type,
			  const int index,
			  const bool hidden);
  void resetTable(const QString &username,
		  const QString &type,
		  const QString &roles);
  void setColumnNames(const QStringList &list);

 private:
  QHash<QString, QList<int> > m_hiddenColumns;
  QVector<QString> m_columnHeaderIndexes;
  void setColumns(const QString &username,
		  const QString &type,
		  const QString &roles);
};

#endif
