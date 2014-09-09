#ifndef _NUMERIC_TABLE_ITEM_H_
#define _NUMERIC_TABLE_ITEM_H_

/*
** -- Qt Includes --
*/

#include <QTableWidgetItem>

class numeric_table_item: public QTableWidgetItem
{
public:
  numeric_table_item(const double);
  bool operator <(const QTableWidgetItem &) const;

private:
  double m_value;
};

#endif
