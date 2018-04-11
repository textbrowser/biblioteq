/*
** -- Local Includes --
*/

#include "biblioteq_numeric_table_item.h"

biblioteq_numeric_table_item::biblioteq_numeric_table_item
(const double value):QTableWidgetItem(QString::number(value, 'f', 2))
{
  m_value = value;
}

bool biblioteq_numeric_table_item::operator <
(const QTableWidgetItem &other) const
{
  return m_value < other.text().toDouble();
}
