/*
** -- Local Includes --
*/

#include "biblioteq_numeric_table_item.h"

/*
** -- numeric_table_item() --
*/

biblioteq_numeric_table_item::biblioteq_numeric_table_item
(const double value):QTableWidgetItem(QString::number(value, 'f', 2))
{
  m_value = value;
}

/*
** -- operator <() --
*/

bool biblioteq_numeric_table_item::operator <
(const QTableWidgetItem &theItem) const
{
  return m_value < theItem.text().toDouble();
}
