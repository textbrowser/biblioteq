#ifndef _BIBLIOTEQ_NUMERIC_TABLE_ITEM_H_
#define _BIBLIOTEQ_NUMERIC_TABLE_ITEM_H_

/*
** -- Qt Includes --
*/

#include <QTableWidgetItem>

class biblioteq_numeric_table_item: public QTableWidgetItem
{
public:
  biblioteq_numeric_table_item(const double value);
  bool operator <(const QTableWidgetItem &other) const;

private:
  double m_value;
};

#endif
